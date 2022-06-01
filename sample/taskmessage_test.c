#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include "../src/em_task.h"

#define TASK_ID_RECV 100
#define TASK_ID_SEND 200

int send_func();
int receive_func();

em_tasksetting_t tasklist[] = {
	{"Receiver", TASK_ID_RECV, 2, 0, 5, receive_func},
	{"Sender", TASK_ID_SEND, 1, 0, 5, send_func}};

em_taskmng_t tm;

//
// Main thread.
//
int main(int argc, char **argv)
{
	int ret;
	int task_num = sizeof(tasklist) / sizeof(em_tasksetting_t);
	printf("task num %d\n", task_num);

	ret = em_init_taskmng(&tm, task_num);

	for (int i = 0; i < task_num; i++)
	{
		ret = em_create_task(&tm, tasklist[i]);
		if (ret)
		{
			printf("pthread_create[Thread %d]\n", i);
			exit(1);
		}
	}

	while (1)
	{
		sleep(1);
	}

	return 0;
}

int send_func()
{
	static int i = 0;
	int ret;

	em_msg_t msg;
	while (1)
	{
		msg.msg_type = 111;
		msg.taskid_to = TASK_ID_RECV;
		msg.taskid_from = TASK_ID_SEND;
		msg.data[0] = i++;
		if (0 != (ret = em_task_msg_send(&tm, msg, 100)))
		{
			printf("[Send] msg send to taskid=5 failed\n");
		}
		sleep(1);
	}
}

int receive_func()
{
	int ret;
	em_msg_t msg;

	while (1)
	{
		if (0 == (ret = em_task_msg_resv(&tm, &msg, EM_NO_TIMEOUT)))
			printf("[Recv] dequeue -> ret:%d id=%d->%d mtype=%d val=%d\n",
				   ret, msg.taskid_from, msg.taskid_to, msg.msg_type, msg.data[0]);
		else{
			usleep(500);
			printf("[Recv] dequeue -> ret:%d timeout\n", ret);
		}
	}
}