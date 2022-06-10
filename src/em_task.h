#ifndef __EM_TASK_H__
#define __EM_TASK_H__

#include "em_datamng.h"
#include "em_queue.h"

typedef int em_taskid_t;

typedef struct
{
	char *task_name; // NULL禁止
	em_taskid_t task_id;
	int priority;	   // 1以上にするとroot権限必要
	size_t stack_size; // 0: default size
	int mqueue_size;
	int (*initialize_func)();
	int (*entry_func)();
} em_tasksetting_t;

typedef struct
{
	pthread_t thread_id;
	char task_name[32];
	em_queue_t msgqueue;
	em_mpool_t msgmpool;

} _em_taskinfo_t;

typedef struct
{
	int msgdata_size;
	em_datamng_t taskinfo_mng;
} em_taskmng_t;

static void *thread_starter(void *func);

int em_init_taskmng(em_taskmng_t *tm,
					int num_max_task,
					int msgdata_size);

int em_task_create_msgqueue(em_taskmng_t *tm,
							em_tasksetting_t tasksetting);

int em_task_initialize_task(em_taskmng_t *tm,
							em_tasksetting_t tasksetting);

int em_task_start_task(em_taskmng_t *tm,
					   em_tasksetting_t tasksetting);

int em_task_create(em_taskmng_t *tm,
				   em_tasksetting_t tasksetting);
				   
int em_task_delete(em_taskmng_t *tm,
				   em_taskid_t task_id);
em_taskid_t em_get_task_id(em_taskmng_t *tm);

// msssage

em_queue_t *_em_msgmng_get_queue(em_taskmng_t *tm,
								 int taskid);

int em_msg_send(em_taskmng_t *tm,
					 int taskid,
					 void *msgdata,
					 int timeout_ms);

int em_msg_recv(em_taskmng_t *tm,
					 void *msgdata,
					 int timeout_ms);

#endif //__EM_TASK_H__
