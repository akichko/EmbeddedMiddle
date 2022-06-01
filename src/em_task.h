#ifndef __EM_TASK_H__
#define __EM_TASK_H__

#include "em_datamng.h"
#include "em_queue.h"
//#include "em_message.h"

typedef int em_taskid_t;

typedef struct
{
	char *task_name;
	em_taskid_t task_id;
	int priority;	   //未対応
	size_t stack_size; // 0: default size
	int mqueue_size;   //未対応
	int (*entry_func)();
} em_tasksetting_t;

typedef struct
{
	pthread_t thread_id;
	em_queue_t mqueue;

} _em_taskinfo_t;

typedef struct
{
	em_datamng_t taskinfo_mng;
	// em_datamng_t thread_task_mng; // ThreadId -> TaskId
} em_taskmng_t;

static void *thread_starter(void *func);

int em_init_taskmng(em_taskmng_t *tm, int num_max_task);
int em_create_task(em_taskmng_t *tm, em_tasksetting_t tasksetting);
int em_delete_task(em_taskmng_t *tm, em_taskid_t task_id);
em_taskid_t em_get_task_id(em_taskmng_t *tm);


//msssage

typedef struct
{
	int msg_type;
	short taskid_to; //削除検討中
	short taskid_from;
	int priority; //未対応
	int data[6];
} em_msg_t;

em_queue_t *_em_msgmng_get_queue(em_taskmng_t *tm,
								 int taskid);

int em_task_msg_send(em_taskmng_t *tm,
					 em_msg_t message,
					 int timeout_ms);

int em_task_msg_resv(em_taskmng_t *tm,
					 em_msg_t *message,
					 int timeout_ms);

#endif //__EM_TASK_H__
