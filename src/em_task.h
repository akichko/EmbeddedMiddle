#ifndef __EM_TASK_H__
#define __EM_TASK_H__

typedef int em_taskid_t;

typedef struct
{
	em_taskid_t task_id;
	int priority; //未対応
	size_t stack_size;
	int (*entry_func)();
} em_tasksetting_t;


static void *thread_starter(void *func);

int em_init_tasks(int num_max_task);
int em_create_task(em_tasksetting_t tasksetting);
int em_delete_task(em_taskid_t task_id);
em_taskid_t em_get_task_id();


#endif //__EM_TASK_H__
