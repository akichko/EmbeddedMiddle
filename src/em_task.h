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
#ifndef __EM_TASK_H__
#define __EM_TASK_H__

#include "em_idatamng.h"
#include "em_queue.h"
#include "em_mutex.h"
//#include "em_eventflag.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int em_taskid_t;

typedef struct
{
	const char *task_name; // NULL禁止
	em_taskid_t task_id;
	int priority;	   // 0-99. 1以上にするとroot権限必要
	size_t stack_size; // 0: default size
	int mqueue_size;
	int (*entry_func)(void *);
	void *entry_func_arg;
} em_tasksetting_t;

typedef struct
{
	pthread_t thread_id;
	char task_name[32];
	em_queue_t msgqueue;
} _em_taskinfo_t;

typedef struct
{
	int msgdata_size;
	em_datamng_t taskinfo_mng;
	void *(*alloc_func)(size_t);
	void (*free_func)(void *);
	em_sem_t sem;
} em_taskmng_t;

typedef struct
{
	int (*entry_func)(void *);
	void *entry_func_arg;
	em_sem_t *sem_ptr;
	void *(*alloc_func)(size_t);
	void (*free_func)(void *);
} em_thrdarg_t;

int em_taskmng_init(em_taskmng_t *tm,
					int max_num_task,
					int msgdata_size,
					void *(*alloc_func)(size_t),
					void (*free_func)(void *));

int em_taskmng_destroy(em_taskmng_t *tm);

int em_task_create_msgqueue(em_taskmng_t *tm,
							em_tasksetting_t tasksetting);

int em_task_start_task(em_taskmng_t *tm,
					   em_tasksetting_t tasksetting);

int em_task_create(em_taskmng_t *tm,
				   em_tasksetting_t tasksetting);

int em_task_delete(em_taskmng_t *tm,
				   em_taskid_t task_id);
				   
em_taskid_t em_get_task_id(em_taskmng_t *tm);

// msssage

int em_msg_send(em_taskmng_t *tm,
				int taskid,
				void *msgdata,
				int timeout_ms);

int em_msg_recv(em_taskmng_t *tm,
				void *msgdata,
				int timeout_ms);

int em_msg_recv_num_check(em_taskmng_t *tm);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_TASK_H__
