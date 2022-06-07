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

int threadfunc_send();
int threadfunc_recv();

em_tasksetting_t tasklist[] = {
	{"Receiver", TASK_ID_RECV, 2, 0, 5, NULL, threadfunc_recv},
	{"Sender", TASK_ID_SEND, 1, 0, 5, NULL, threadfunc_send}};

typedef struct
{
	int msg_type;
	short taskid_to;
	short taskid_from;
	int data[4];
} testmsg_t;

em_taskmng_t tm;

//
// Main thread.
//
int main(int argc, char **argv)
{
	int ret;
	int task_num = sizeof(tasklist) / sizeof(em_tasksetting_t);
	printf("task num %d\n", task_num);

	if (0 != em_init_taskmng(&tm, task_num, sizeof(testmsg_t)))
	{
		printf("em_init_taskmng error\n");
		exit(1);
	}

	for (int i = 0; i < task_num; i++)
	{
		if (0 != em_task_create(&tm, tasklist[i]))
		{
			printf("em_task_create error [%d]\n", i);
			exit(1);
		}
	}

	sleep(3);

	for (int i = 0; i < task_num; i++)
	{
		if (0 != em_task_delete(&tm, tasklist[i].task_id))
		{
			printf("em_task_delete error [%d]\n", i);
			exit(1);
		}
	}

	return 0;
}

int threadfunc_send()
{
	int i = 0;
	int ret;

	testmsg_t msg;
	while (1)
	{
		msg.msg_type = 111;
		msg.taskid_to = TASK_ID_RECV;
		msg.taskid_from = TASK_ID_SEND;
		msg.data[0] = i++;
		if (0 != (ret = em_msg_send(&tm, TASK_ID_RECV, &msg, 100)))
		{
			printf("[Send] msg send to taskid=%d failed\n", TASK_ID_RECV);
		}
		if (i == 8)
		{
			printf("[Send] break\n");
			break;
		}
		sleep(1);
	}
	return i;
}

int threadfunc_recv()
{
	int i = 0;
	int ret;
	testmsg_t msg;

	while (1)
	{
		if (0 == (ret = em_msg_resv(&tm, &msg, 1100)))
			printf("[Recv] dequeue -> ret:%d id=%d->%d mtype=%d val=%d\n",
				   ret, msg.taskid_from, msg.taskid_to, msg.msg_type, msg.data[0]);
		else
		{
			usleep(500);
			printf("[Recv] dequeue -> ret:%d timeout\n", ret);
			if (++i == 4)
			{
				printf("[Recv] break\n");
				break;
			}
		}
	}
	return i;
}