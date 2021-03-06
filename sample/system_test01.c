#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/em_system.h"

#define TASK_ID_APP1 100
#define TASK_ID_APP2 200

typedef struct
{
	int msg_type;
	int data;
} testmsg_t;

typedef struct
{
	int task_to;
	int data;
} timer_arg_t;

int app1_init(void *arg);
int app2_init(void *arg);
int app1_signal(int arg);
int app2_signal(int arg);
int app1_main();
int app2_main();
void timer_func(void *arg);
void cmd_shutdown(int argc, char **argv);

em_memmng_t memmng;
em_sysmng_t sysmng;
em_datamng_t dm;

em_systaskinfo_t systaskstg[] = {
	//{{"Cmd", TASK_ID_CMD, 0, 0, 5, cmd_main}, NULL, NULL},
	{{"App1", TASK_ID_APP1, 0, 0, 256, app1_main}, &app1_init, &app1_signal},
	{{"App2", TASK_ID_APP2, 0, 0, 256, app2_main}, &app2_init, &app2_signal}};

int b_shutdown1 = 0;
int b_shutdown2 = 0;

em_sysmng_stg_t sys_setting = {0};
em_cmdsetting_t shutdowncmd_setting = {1, "shutdown", &cmd_shutdown};

void *local_malloc(size_t size)
{
	return em_malloc(&memmng, size);
}

void local_free(void *addr)
{
	em_free(&memmng, addr);
}

void cmd_shutdown(int argc, char **argv)
{
	printf("==== shutdown exec! ====\n");

	em_memmng_print(&memmng, TRUE);
	// signal
	int task_num = sizeof(systaskstg) / sizeof(em_systaskinfo_t);
	for (int i = 0; i < task_num; i++)
	{
		if (NULL != systaskstg[i].system_cbfunc)
		{
			systaskstg[i].system_cbfunc(0);
		}
	}

	em_cmd_stop(&sysmng.cmdmng);
}

int app1_init(void *arg)
{
	printf("app1 init\n");
	return 0;
}

int app2_init(void *arg)
{
	printf("app2 init\n");
	return 0;
}

int app1_signal(int arg)
{
	b_shutdown1 = 1;
	printf("app1 signal\n");
	return 0;
}

int app2_signal(int arg)
{
	b_shutdown2 = 1;
	printf("app2 signal\n");
	return 0;
}

int app1_main()
{
	testmsg_t msg;
	timer_arg_t timerarg = {TASK_ID_APP1, 10};

	em_timersetting_t timersetting = {100, 1000, timer_func, &timerarg};
	if (0 != em_timer_create(&sysmng.tmrmng, &timersetting))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
	printf("[App1] timer created\n");

	while (!b_shutdown1)
	{
		em_msg_recv(&sysmng.tskmng, &msg, EM_NO_TIMEOUT);

		printf("[App1] msgType=%d, data=%d\n", msg.msg_type, msg.data);
	}

	if (0 != em_timer_delete(&sysmng.tmrmng, timersetting.timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	printf("[App1] timer deleted\n");

	return 0;
}

int app2_main()
{
	testmsg_t msg;
	timer_arg_t timerarg = {TASK_ID_APP2, 20};

	em_timersetting_t timersetting = {200, 2500, timer_func, &timerarg};
	if (0 != em_timer_create(&sysmng.tmrmng, &timersetting))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
	printf("[App2] timer created\n");

	while (!b_shutdown2)
	{
		em_msg_recv(&sysmng.tskmng, &msg, EM_NO_TIMEOUT);
		printf("[App2] msgType=%d, data=%d\n", msg.msg_type, msg.data);

		memset(&msg, 0, sizeof(testmsg_t));
		msg.msg_type = 21;
		msg.data = 100;
		em_msg_send(&sysmng.tskmng, TASK_ID_APP1, &msg, 1000);
	}

	if (0 != em_timer_delete(&sysmng.tmrmng, timersetting.timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	printf("[App2] timer deleted\n");

	return 1;
}

void timer_func(void *arg)
{
	timer_arg_t *timer_arg = (timer_arg_t *)arg;
	printf("timer_func \n");
	testmsg_t msg;
	msg.msg_type = timer_arg->data;
	msg.data = em_get_tick_count(&sysmng.timemng);
	em_msg_send(&sysmng.tskmng, timer_arg->task_to, &msg, 1000);
}

int init()
{
	int max_alloc_num = 1000;
	int mem_total_size = 1024 * 1024;
	int mem_unit_size = 1024;

	sys_setting.max_num_mutex = 100;
	sys_setting.max_num_sem = 100;
	sys_setting.max_num_timer = 2;
	sys_setting.max_num_cmd = 5;
	sys_setting.max_num_task = 2;
	sys_setting.msgdata_size = sizeof(testmsg_t);
	sys_setting.alloc_func = &local_malloc;
	sys_setting.free_func = &local_free;

	if (0 != em_memmng_create(&memmng, mem_total_size, mem_unit_size, max_alloc_num))
	{
		em_printf(EM_LOG_ERROR, "memmng init error\n");
		return -1;
	}

	if (0 != em_sysmng_init(&sysmng, &sys_setting))
	{
		printf("sys init error\n");
		return -1;
	}
	em_datamng_create(&dm, 128, 128, EM_DMNG_DPLCT_ERROR, &local_malloc, &local_free);

	em_memmng_print(&memmng, TRUE);
	return 0;
}

int finalize()
{
	if (0 != em_sysmng_finalize(&sysmng))
	{
		printf("sys init error\n");
		return -1;
	}
	em_datamng_delete(&dm);

	em_memmng_print(&memmng, TRUE);

	if (0 != em_memmng_delete(&memmng))
	{
		em_printf(EM_LOG_ERROR, "memmng init error\n");
		return -1;
	}

	return 0;
}

int main()
{
	// system initialize
	if (0 != init())
	{
		printf("init error\n");
		return -1;
	}

	// shutdown command
	if (0 != em_cmd_regist(&sysmng.cmdmng, &shutdowncmd_setting))
	{
		printf("error\n");
	}

	// task initialize
	int task_num = sizeof(systaskstg) / sizeof(em_systaskinfo_t);
	for (int i = 0; i < task_num; i++)
	{
		if (NULL != systaskstg[i].initialize_func)
		{
			systaskstg[i].initialize_func(NULL);
		}
	}

	// task start
	for (int i = 0; i < task_num; i++)
	{
		if (0 != em_task_create(&sysmng.tskmng, systaskstg[i].task_stg))
		{
			printf("create task error\n");
			exit(1);
		}
	}

	em_cmd_start(&sysmng.cmdmng);

	for (int i = 0; i < task_num; i++)
	{
		if (0 != em_task_delete(&sysmng.tskmng, systaskstg[i].task_stg.task_id))
		{
			printf("delete task error\n");
			exit(1);
		}
	}

	// system initialize
	if (0 != finalize())
	{
		printf("finalize error\n");
		return -1;
	}

	printf("[Main] system halted\n");
	return 0;
}
