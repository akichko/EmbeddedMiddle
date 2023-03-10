#include <stdio.h>
#include <stdlib.h>

#include "../src/em_task.h"
#include "../src/em_time.h"
#include "../src/em_timer.h"
#include "../src/em_command.h"

em_timemng_t timemng;
em_timermng_t tmrmng;
em_taskmng_t tskmng;
em_cmdmng_t cmdmng;

#define TASK_ID_APP1 100

int app1_main();
void timer_func(void *arg);
int shutdown(int argc, char **argv);

em_tasksetting_t tasksetting =
	{"App1", TASK_ID_APP1, 0, 0, 5, app1_main};

int b_shutdown = 0;

int cmd_shutdown(int argc, char **argv)
{
	printf("shutdown exec\n");
	b_shutdown = 1;
	em_cmd_stop(&cmdmng);
	return 0;
}

typedef struct
{
	int msg_type;
	int data;
} testmsg_t;

int app1_main()
{
	testmsg_t msg;
	em_timersetting_t timersetting = {1000, timer_func, NULL};
	uint timer_id;
	if (0 != em_timer_create(&tmrmng, &timersetting, &timer_id))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
	printf("[App1] timer created\n");

	while (!b_shutdown)
	{
		em_msg_recv(&tskmng, &msg, EM_NO_TIMEOUT);

		printf("[App1] msgType=%d, data=%d\n", msg.msg_type, msg.data);
	}

	if (0 != em_timer_delete(&tmrmng, timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	printf("[App1] timer deleted\n");

	printf("[App1] task ended\n");
	return 0;
}

void timer_func(void *arg)
{
	printf("timer_func \n");
	testmsg_t msg;
	msg.msg_type = 100;
	msg.data = em_get_tick_count(&timemng);
	em_msg_send(&tskmng, TASK_ID_APP1, &msg, 1000);
}

int main()
{
	//int ret;
	
	if (0 != em_tick_init(&timemng))
	{
		printf("error\n");
	}

	if (0 != em_timermng_init(&tmrmng, 10, &malloc, &free))
	{
		printf("error\n");
	}

	if (0 != em_cmd_init(&cmdmng, 10, &malloc, &free))
	{
		printf("error\n");
	}

	// shutdown command
	em_cmdsetting_t shutdowncmd_setting = {"shutdown", &cmd_shutdown};
	if (0 != em_cmd_register(&cmdmng, &shutdowncmd_setting))
	{
		printf("error\n");
	}

	if (0 != em_taskmng_init(&tskmng, 1, sizeof(testmsg_t), &malloc, &free))
	{
		printf("error\n");
		exit(1);
	}

	if (0 != em_task_create(&tskmng, tasksetting))
	{
		printf("create task error\n");
		exit(1);
	}

	em_cmd_start(&cmdmng);

	if (0 != em_task_delete(&tskmng, tasksetting.task_id))
	{
		printf("em_task_delete error\n");
		exit(1);
	}

	printf("[Main] system halted\n");
	return 0;
}
