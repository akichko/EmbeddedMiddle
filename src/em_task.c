#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include "em_task.h"

static void *thread_starter(void *func)
{
	int *ret = malloc(sizeof(int)); // Allocate a return value area.
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

int em_init_taskmng(em_taskmng_t *tm, int num_max_task)
{
	em_datamng_create(&tm->taskinfo_mng, sizeof(_em_taskinfo_t), num_max_task);
	// em_datamng_create(&tm->task_thread_mng, sizeof(pthread_t), num_max_task);
	// em_datamng_create(&tm->thread_task_mng, sizeof(em_taskid_t), num_max_task);
}

int em_task_create_msgqueue(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	_em_taskinfo_t *task_info = (_em_taskinfo_t *)em_datamng_get_data_ptr(&tm->taskinfo_mng, tasksetting.task_id);
	if (task_info != NULL)
	{
		em_queue_create(&task_info->msgqueue, sizeof(em_msg_t), tasksetting.mqueue_size);
	}
	else //新規作成
	{
		_em_taskinfo_t newtask_info;
		em_queue_create(&newtask_info.msgqueue, sizeof(em_msg_t), tasksetting.mqueue_size);
		em_datamng_add_data(&tm->taskinfo_mng, tasksetting.task_id, &newtask_info);
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

	ret = pthread_attr_init(&tattr);
	pthread_attr_setstacksize(&tattr, 0);
	// pthread_setschedprio(&tattr, 0);
	ret = pthread_create(&thread_id, &tattr, thread_starter, (void *)tasksetting.entry_func);
	if (ret)
	{
		printf("pthread_create[Thread %d]\n", tasksetting.task_id);
		return -1;
	}
	printf("TaskId %d created. threadId=%ld\n", tasksetting.task_id, thread_id);

	_em_taskinfo_t *task_info = (_em_taskinfo_t *)em_datamng_get_data_ptr(&tm->taskinfo_mng, tasksetting.task_id);
	if (task_info != NULL)
	{
		task_info->thread_id = thread_id;
	}
	else //新規作成
	{
		_em_taskinfo_t newtask_info;
		newtask_info.thread_id = thread_id;
		em_datamng_add_data(&tm->taskinfo_mng, tasksetting.task_id, &newtask_info);
	}

	return 0;
}

int em_create_task(em_taskmng_t *tm, em_tasksetting_t tasksetting)
{
	int ret;
	pthread_attr_t tattr;
	// pthread_t thread_id; //保存用
	_em_taskinfo_t newtask_info;

	ret = pthread_attr_init(&tattr);
	pthread_attr_setstacksize(&tattr, 0);
	// pthread_setschedprio(&tattr, 0);
	ret = pthread_create(&newtask_info.thread_id, &tattr, thread_starter, (void *)tasksetting.entry_func);
	if (ret)
	{
		printf("pthread_create[Thread %d]\n", tasksetting.task_id);
		return -1;
	}
	printf("TaskId %d created. threadId=%ld\n", tasksetting.task_id, newtask_info.thread_id);

	em_queue_create(&newtask_info.msgqueue, sizeof(em_msg_t), tasksetting.mqueue_size);

	em_datamng_add_data(&tm->taskinfo_mng, tasksetting.task_id, &newtask_info);
	// em_datamng_add_data(&tm->task_thread_mng, tasksetting.task_id, &thread_id);
	// em_datamng_add_data(&tm->thread_task_mng, newtask_info.thread_id, &tasksetting.task_id);

	return 0;
}

int em_delete_task(em_taskmng_t *tm, em_taskid_t task_id)
{
	int ret;
	_em_taskinfo_t taskinfo;

	void *th_ret;

	ret = em_datamng_get_data(&tm->taskinfo_mng, task_id, &taskinfo);
	if (ret != 0)
	{
		printf("task %d not found\n", task_id);
	}

	pthread_join(taskinfo.thread_id, &th_ret);

	printf("TaskId %d (id=%ld) stopped. ret=%d \n", task_id, taskinfo.thread_id, *(int *)th_ret);

	if (th_ret != NULL)
	{
		free(th_ret); // Free return value memory.
		// em_datamng_remove_data(&tm->thread_task_mng, taskinfo.thread_id);
		em_datamng_remove_data(&tm->taskinfo_mng, task_id);
	}

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
		printf("task id %d not found (thread id = %ld)\n", task_id, thread_id);
		return -1;
	}
	// printf("thred_id: %ld -> task_id: %d\n", thread_id, task_id);
	return task_id;
}

em_queue_t *_em_msgmng_get_queue(em_taskmng_t *tm, int taskid)
{
	_em_taskinfo_t *taskinfo = em_datamng_get_data_ptr(&tm->taskinfo_mng, taskid);
	if (taskinfo == NULL)
	{
		printf("taskinfo of id %d not found\n", taskid);
		return NULL;
	}
	return &taskinfo->msgqueue;
}

int em_task_msg_send(em_taskmng_t *tm, em_msg_t message, int timeout_ms)
{
	em_queue_t *mqueue = _em_msgmng_get_queue(tm, message.taskid_to);
	if (mqueue == NULL)
	{
		return -1;
	}
	return em_enqueue(mqueue, &message, timeout_ms);
}

int em_task_msg_resv(em_taskmng_t *tm, em_msg_t *message, int timeout_ms)
{
	int taskid = em_get_task_id(tm);
	em_queue_t *mqueue = _em_msgmng_get_queue(tm, taskid);
	if (mqueue == NULL)
	{
		printf("_em_msgmng_get_queue error\n");
		return -1;
	}

	return em_dequeue(mqueue, message, timeout_ms);
}
