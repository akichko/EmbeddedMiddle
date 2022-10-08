/*============================================================================
MIT License

Copyright (c) 2022 akichko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include "em_task.h"
#include "em_print.h"

static void *_em_thread_starter(void *thrdarg)
{
	em_thrdarg_t *arg = (em_thrdarg_t *)thrdarg;

	int *ret = (int *)arg->alloc_func(sizeof(int)); // Allocate a return value area.

	int (*funcptr)() = arg->entry_func;

	//開始同期制御
	// em_printf(EM_LOG_INFO, "event wait start\n");
	em_sem_wait(arg->sem_ptr, EM_WAIT);
	// em_printf(EM_LOG_INFO, "event wait end\n");

	*ret = (*funcptr)();

	arg->free_func(thrdarg);

	pthread_exit(ret);
}

static char _em_threadid_comparator(void *dm_data, void *thread_id)
{
	if (*(ulong *)thread_id == ((_em_taskinfo_t *)dm_data)->thread_id)
	{
		return 1;
	}
	return 0;
}

int em_taskmng_init(em_taskmng_t *tm, int max_num_task, int msgdata_size,
					void *(*alloc_func)(size_t),
					void (*free_func)(void *))
{
	tm->alloc_func = alloc_func;
	tm->free_func = free_func;
	tm->msgdata_size = msgdata_size;
	if (0 != em_sem_init(&tm->sem, 0))
	{
		return -1;
	}
	return em_datamng_create(&tm->taskinfo_mng, sizeof(_em_taskinfo_t), max_num_task, EM_DMNG_DPLCT_ERROR, alloc_func, free_func);
}

int em_taskmng_destroy(em_taskmng_t *tm)
{
	if (0 != em_sem_destroy(&tm->sem))
	{
		return -1;
	}
	return em_datamng_delete(&tm->taskinfo_mng);
}

int em_task_create_msgqueue(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	if (tm->msgdata_size <= 0)
	{
		// message not available
		return 0;
	}
	_em_taskinfo_t *task_info = (_em_taskinfo_t *)em_datamng_get_data_ptr(&tm->taskinfo_mng, tasksetting.task_id);
	if (task_info != NULL) //タスク登録済み
	{
		return em_queue_create(&task_info->msgqueue, tm->msgdata_size, tasksetting.mqueue_size, tm->alloc_func, tm->free_func);
	}
	else //タスク未登録⇒新規作成
	{
		_em_taskinfo_t newtask_info;
		if (0 != em_queue_create(&newtask_info.msgqueue, tm->msgdata_size, tasksetting.mqueue_size, tm->alloc_func, tm->free_func))
		{
			return -1;
		}
		return em_datamng_add_data(&tm->taskinfo_mng, tasksetting.task_id, &newtask_info);
	}
}

// int em_task_initialize_task(em_taskmng_t *tm, em_tasksetting_t tasksetting)
//{
//	if (tasksetting.initialize_func != NULL)
//	{
//		return tasksetting.initialize_func();
//	}
//
//	return 0;
// }

int em_task_start_task(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	pthread_attr_t tattr;
	pthread_t thread_id;
	struct sched_param scheprm;
	em_thrdarg_t *thrdarg = (em_thrdarg_t*)tm->alloc_func(sizeof(em_thrdarg_t));

	thrdarg->sem_ptr = &tm->sem;
	thrdarg->entry_func = tasksetting.entry_func;
	thrdarg->alloc_func = tm->alloc_func;
	thrdarg->free_func = tm->free_func;

	_em_taskinfo_t *task_info = (_em_taskinfo_t *)em_datamng_get_data_ptr(&tm->taskinfo_mng, tasksetting.task_id);

	if (task_info == NULL && tm->taskinfo_mng.mp.num_used == tm->taskinfo_mng.mp.num_max)
	{
		em_printf(EM_LOG_ERROR, "error: task num max\n");
		return -1;
	}

	if (0 != pthread_attr_init(&tattr))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_init\n");
		return -1;
	}
	if (tasksetting.stack_size > 0 && 0 != pthread_attr_setstacksize(&tattr, tasksetting.stack_size))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setstacksize\n");
		return -1;
	}
	if (0 != pthread_attr_setschedpolicy(&tattr, SCHED_FIFO))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setschedpolicy\n");
		return -1;
	}
	// should be execed by root
	if (tasksetting.priority > 0 && 0 != pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setinheritsched\n");
		return -1;
	}
	// pthread_setschedprio(&tattr, 0);
	scheprm.sched_priority = tasksetting.priority;
	if (tasksetting.priority > 0 && 0 != pthread_attr_setschedparam(&tattr, &scheprm))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setschedparam\n");
		return -1;
	}
	if (0 != pthread_create(&thread_id, &tattr, _em_thread_starter, (void *)thrdarg))
	{
		em_printf(EM_LOG_ERROR, "pthread_create error [TaskId=%d]\n", tasksetting.task_id);
		return -1;
	}
	em_printf(EM_LOG_TOP, "TaskId %d (%s) created. threadId=%ld\n", tasksetting.task_id, tasksetting.task_name, thread_id);

	if (task_info != NULL)
	{
		// int taskname_len = strlen(tasksetting.task_name);
		memcpy(task_info->task_name, tasksetting.task_name, strlen(tasksetting.task_name) + 1);
		task_info->thread_id = thread_id;
	}
	else //新規作成
	{
		_em_taskinfo_t newtask_info;
		memcpy(newtask_info.task_name, tasksetting.task_name, strlen(tasksetting.task_name) + 1);
		newtask_info.thread_id = thread_id;
		em_datamng_add_data(&tm->taskinfo_mng, tasksetting.task_id, &newtask_info);
	}

	if (0 != em_sem_post(&tm->sem))
	{
		em_printf(EM_LOG_ERROR, "em sync error\n");
	}

	return 0;
}

int em_task_create(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	if (tm->taskinfo_mng.mp.num_used == tm->taskinfo_mng.mp.num_max)
	{
		em_printf(EM_LOG_ERROR, "task num max\n");
		return -1;
	}
	if (0 != em_task_create_msgqueue(tm, tasksetting))
	{
		em_printf(EM_LOG_ERROR, "create msgqueue error\n");
		return -1;
	}

	// if (0 != em_task_initialize_task(tm, tasksetting))
	//{
	//	em_printf(EM_LOG_ERROR, "task initialize error\n");
	//	return -1;
	// }

	if (0 != em_task_start_task(tm, tasksetting))
	{
		em_printf(EM_LOG_ERROR, "task start error\n");
		return -1;
	}

	return 0;
}

int em_task_delete(em_taskmng_t *tm, em_taskid_t task_id)
{
	int ret;
	_em_taskinfo_t taskinfo;

	void *th_ret;

	ret = em_datamng_get_data(&tm->taskinfo_mng, task_id, &taskinfo);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "task %d not found\n", task_id);
	}

	pthread_join(taskinfo.thread_id, &th_ret);

	em_printf(EM_LOG_INFO, "TaskId %d (%s tid=%ld) stopped. ret=%d \n", task_id, taskinfo.task_name, taskinfo.thread_id, *(int *)th_ret);

	if (th_ret != NULL)
	{
		tm->free_func(th_ret); // Free return value memory.
	}

	em_queue_delete(&taskinfo.msgqueue);

	em_datamng_remove_data(&tm->taskinfo_mng, task_id);

	return 0;
}

em_taskid_t em_get_task_id(em_taskmng_t *tm)
{
	pthread_t thread_id = pthread_self();
	em_taskid_t task_id;
	// ret = em_datamng_get_data(&tm->thread_task_mng, thread_id, &task_id);
	// if (ret != 0)
	task_id = em_datamng_get_id_by_func(&tm->taskinfo_mng, &thread_id, &_em_threadid_comparator);
	if (task_id < 0)
	{
		em_printf(EM_LOG_ERROR, "task id %d not found (thread id = %ld)\n", task_id, thread_id);
		return -1;
	}
	// em_printf(EM_LOG_ERROR, "thred_id: %ld -> task_id: %d\n", thread_id, task_id);
	return task_id;
}

em_queue_t *_em_msgmng_get_queue(em_taskmng_t *tm, int taskid)
{
	_em_taskinfo_t *taskinfo = (_em_taskinfo_t *)em_datamng_get_data_ptr(&tm->taskinfo_mng, taskid);
	if (taskinfo == NULL)
	{
		em_printf(EM_LOG_ERROR, "taskinfo of id %d not found\n", taskid);
		return NULL;
	}
	return &taskinfo->msgqueue;
}

int em_msg_send(em_taskmng_t *tm, int taskid, void *msgdata, int timeout_ms)
{
	em_queue_t *mqueue = _em_msgmng_get_queue(tm, taskid);
	if (mqueue == NULL)
	{
		em_printf(EM_LOG_ERROR, "msgqueue not found taskid=%d\n", taskid);
		return -1;
	}

	if (0 != em_enqueue(mqueue, msgdata, timeout_ms))
	{
		em_printf(EM_LOG_ERROR, "msg send failed\n");
	}
	return 0;
}

int em_msg_recv(em_taskmng_t *tm, void *msgdata, int timeout_ms)
{
	int taskid = em_get_task_id(tm);

	em_queue_t *mqueue = _em_msgmng_get_queue(tm, taskid);
	if (mqueue == NULL)
	{
		em_printf(EM_LOG_ERROR, "msgqueue not found taskid=%d\n", taskid);
		return -1;
	}

	if (0 != em_dequeue(mqueue, msgdata, timeout_ms))
	{
		// em_printf(EM_LOG_ERROR, "recv timeout\n");
		return -1;
	}

	return 0;
}
