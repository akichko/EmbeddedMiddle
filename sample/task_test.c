#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include "../src/em_task.h"

int thread1(); // Thread Entry function 1
int thread2(); // Thread Entry function 2

#define TASK_ID_APP1 100
#define TASK_ID_APP2 200
#define TASK_ID_APP3 300

em_tasksetting_t tasklist[] = {
	{"App1", TASK_ID_APP1, 0, 0, 5, thread1},
	{"App2", TASK_ID_APP2, 0, 0, 5, thread2},
	{"App3", TASK_ID_APP3, 0, 0, 5, thread1}};

em_taskmng_t tm;

//
// Main thread.
//
int main(int argc, char **argv)
{
	int ret;
	int task_num = sizeof(tasklist) / sizeof(em_tasksetting_t);
	printf("task num %d\n", task_num);

	ret = em_taskmng_init(&tm, task_num, sizeof(int), &malloc, &free);

	for (int i = 0; i < task_num; i++)
	{
		ret = em_task_create(&tm, tasklist[i]);
		if (ret)
		{
			printf("pthread_create[Thread %d]\n", i);
			exit(1);
		}
	}

	for (int i = 0; i < task_num; i++)
	{
		ret = em_task_delete(&tm, tasklist[i].task_id);
		if (ret != 0)
		{
			printf("em_delete_task Error\n");
			exit(2);
		}
	}
	return 0;
}


// Thread entry function 1
int thread1()
{
	printf("Thread1 started.\n");
	int t = 3;
	sleep(t);
	printf("Thread1 (task id = %d) ended. %ds\n", em_get_task_id(&tm), t);

	return 1;
}

// Thread entry function 2
int thread2()
{
	printf("Thread2 started.\n");

	int t = 1;
	sleep(t);
	printf("Thread2 (task id = %d) ended. %ds\n", em_get_task_id(&tm), t);

	return 2;
}