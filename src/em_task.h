#ifndef __EM_TASK_H__
#define __EM_TASK_H__

#include "em_datamng.h"

typedef int em_taskid_t;

typedef struct
{
	em_taskid_t task_id;
	int priority; //未対応
	size_t stack_size; // 0: default size
	int (*entry_func)();
} em_tasksetting_t;

typedef struct
{
	em_datamng_t task_thread_mng; //TaskId -> ThreadId
	em_datamng_t thread_task_mng; //ThreadId -> TaskId
} em_taskmng_t;

static void *thread_starter(void *func);

int em_init_tasks(em_taskmng_t* tm, int num_max_task);
int em_create_task(em_taskmng_t* tm, em_tasksetting_t tasksetting);
int em_delete_task(em_taskmng_t* tm, em_taskid_t task_id);
em_taskid_t em_get_task_id(em_taskmng_t* tm);

#endif //__EM_TASK_H__
