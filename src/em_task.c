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

int em_init_tasks(em_taskmng_t* tm, int num_max_task)
{
	em_create_datamng(&tm->task_thread_mng, sizeof(pthread_t), num_max_task);
	em_create_datamng(&tm->thread_task_mng, sizeof(em_taskid_t), num_max_task);
}

int em_create_task(em_taskmng_t* tm, em_tasksetting_t tasksetting)
{
	int ret;
	pthread_attr_t tattr;
	pthread_t thread_id; //保存用

	ret = pthread_attr_init(&tattr);
	pthread_attr_setstacksize(&tattr, 0);
	// pthread_setschedprio(&tattr, 0);
	ret = pthread_create(&thread_id, &tattr, thread_starter, (void *)tasksetting.entry_func);
	if (ret)
	{
		printf("pthread_create[Thread %d]\n", tasksetting.task_id);
		exit(1);
	}
	printf("TaskId %d created. threadId=%ld\n", tasksetting.task_id, thread_id);

	em_set_data(&tm->task_thread_mng, &thread_id, tasksetting.task_id);
	em_set_data(&tm->thread_task_mng, &tasksetting.task_id, thread_id);

	return 0;
}

int em_delete_task(em_taskmng_t* tm, em_taskid_t task_id)
{
	int ret;
	pthread_t thread_id;

	void *th_ret;

	ret = em_get_data(&tm->task_thread_mng, task_id, &thread_id);
	if (ret != 0)
	{
		printf("task %d not found\n", task_id);
	}

	pthread_join(thread_id, &th_ret);

	printf("TaskId %d (id=%ld) stopped. ret=%d \n", task_id, thread_id, *(int *)th_ret);
	// printf("Thread %d stopped [%d]\n", i, *(int *)th_ret);
	if (th_ret != NULL)
	{
		free(th_ret); // Free return value memory.
		em_del_block(&tm->task_thread_mng, task_id);
		em_del_block(&tm->thread_task_mng, thread_id);
	}

	return 0;
}

em_taskid_t em_get_task_id(em_taskmng_t* tm)
{
	int ret;
	pthread_t thread_id = pthread_self();
	em_taskid_t task_id;

	ret = em_get_data(&tm->thread_task_mng, thread_id, &task_id);
	if (ret != 0)
	{
		printf("task %d not found\n", task_id);
		return -1;
	}
	return task_id;
}
