#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include "em_task.h"
#include "em_print.h"

static void *_em_thread_starter(void *func)
{
	int *ret = (int *)malloc(sizeof(int)); // Allocate a return value area.

	int (*funcptr)() = func;
	*ret = (*funcptr)();

	pthread_exit(ret);
}

static char _em_threadid_comparator(void *dm_data, void *thread_id)
{
	if (*(unsigned long *)thread_id == ((_em_taskinfo_t *)dm_data)->thread_id)
	{
		return 1;
	}
	return 0;
}

int em_init_taskmng(em_taskmng_t *tm, int num_max_task, int msgdata_size,
					void *(*allc_func)(size_t),
					void (*free_func)(void *))
{
	tm->free_func = free_func;
	tm->msgdata_size = msgdata_size;
	return em_datamng_create(&tm->taskinfo_mng, sizeof(_em_taskinfo_t), num_max_task, allc_func, free_func);
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
		em_queue_create(&task_info->msgqueue, tm->msgdata_size, tasksetting.mqueue_size);
	}
	else //タスク未登録⇒新規作成
	{
		_em_taskinfo_t newtask_info;
		em_queue_create(&newtask_info.msgqueue, tm->msgdata_size, tasksetting.mqueue_size);
	}

	return 0;
}

int em_task_initialize_task(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	if (tasksetting.initialize_func != NULL)
	{
		return tasksetting.initialize_func();
	}

	return 0;
}

int em_task_start_task(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	int ret;
	pthread_attr_t tattr;
	pthread_t thread_id;
	struct sched_param scheprm;

	if (0 != pthread_attr_init(&tattr))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_init\n");
	}
	if (tasksetting.stack_size > 0 && 0 != pthread_attr_setstacksize(&tattr, tasksetting.stack_size))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setstacksize\n");
	}
	if (0 != pthread_attr_setschedpolicy(&tattr, SCHED_FIFO))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setschedpolicy\n");
	}
	// should be execed by root
	if (tasksetting.priority > 0 && 0 != pthread_attr_setinheritsched(&tattr, PTHREAD_EXPLICIT_SCHED))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setinheritsched\n");
	}
	// pthread_setschedprio(&tattr, 0);
	scheprm.sched_priority = tasksetting.priority;
	if (tasksetting.priority > 0 && 0 != pthread_attr_setschedparam(&tattr, &scheprm))
	{
		em_printf(EM_LOG_ERROR, "error: pthread_attr_setschedparam\n");
	}
	if (0 != pthread_create(&thread_id, &tattr, _em_thread_starter, (void *)tasksetting.entry_func))
	{
		em_printf(EM_LOG_ERROR, "pthread_create error [TaskId=%d]\n", tasksetting.task_id);
		return -1;
	}
	em_printf(EM_LOG_INFO, "TaskId %d (%s) created. threadId=%ld\n", tasksetting.task_id, tasksetting.task_name, thread_id);

	_em_taskinfo_t *task_info = (_em_taskinfo_t *)em_datamng_get_data_ptr(&tm->taskinfo_mng, tasksetting.task_id);
	if (task_info != NULL)
	{
		// int taskname_len = strlen(tasksetting.task_name);
		memcpy(task_info->task_name, tasksetting.task_name, strlen(tasksetting.task_name));
		task_info->thread_id = thread_id;
	}
	else //新規作成
	{
		_em_taskinfo_t newtask_info;
		memcpy(newtask_info.task_name, tasksetting.task_name, strlen(tasksetting.task_name));
		newtask_info.thread_id = thread_id;
		em_datamng_add_data(&tm->taskinfo_mng, tasksetting.task_id, &newtask_info);
	}

	return 0;
}

int em_task_create(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	int ret;

	if (0 != em_task_create_msgqueue(tm, tasksetting))
	{
		em_printf(EM_LOG_ERROR, "create msgqueue error\n");
		return -1;
	}
	if (0 != em_task_initialize_task(tm, tasksetting))
	{
		em_printf(EM_LOG_ERROR, "task initialize error\n");
		return -1;
	}

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
		free(th_ret); // Free return value memory.
	}

	em_datamng_remove_data(&tm->taskinfo_mng, task_id);

	return 0;
}

em_taskid_t em_get_task_id(em_taskmng_t *tm)
{
	int ret;
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
	_em_taskinfo_t *taskinfo = em_datamng_get_data_ptr(&tm->taskinfo_mng, taskid);
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

	return em_enqueue(mqueue, msgdata, timeout_ms);
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
