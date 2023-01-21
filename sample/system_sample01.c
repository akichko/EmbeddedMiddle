#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/em_system.h"

#define TASK_ID_APP1 100
#define TASK_ID_APP2 200
#define TASK_ID_CMD 300

typedef enum
{
	EVENT_TIMER = 0,
	EVENT_OTHER,
	EVENT_MAXNUM
} e_event_no;

typedef struct
{
	int msg_type;
	int data;
} testmsg_t;

typedef struct tag_timer_arg_msg
{
	int task_to;
	int data;
} timer_arg_msg_t;

typedef struct tag_timer_arg_evt
{
	e_event_no event_no;
} timer_arg_evt_t;

typedef struct tag_em_systaskinfo
{
	em_tasksetting_t task_stg;
	int (*initialize_func)(void *);
	int (*system_cbfunc)(int);
} em_systaskinfo_t;

int app1_init(void *arg);
int app2_init(void *arg);
int app1_signal(int arg);
int app2_signal(int arg);
int app1_main();
int app2_main();
static int cmd_main();
static void timer_func_msg(void *arg);
static void timer_func_evt(void *arg);
void cmd_shutdown(int argc, char **argv);
void cmd_pause(int argc, char **argv);

em_memmng_t memmng;
em_sysmng_t sysmng;
em_datamng_t dm;

em_systaskinfo_t systaskstg[] = {
	{{"App1", TASK_ID_APP1, 0, 0, 16, app1_main}, &app1_init, &app1_signal},
	{{"App2", TASK_ID_APP2, 0, 0, 16, app2_main}, &app2_init, &app2_signal},
	{{"Cmd", TASK_ID_CMD, 0, 0, 5, cmd_main}, NULL, NULL}};

int b_shutdown1 = 0;
int b_shutdown2 = 0;
int b_shutdown = 0;

int b_pause1 = 0;
int b_pause2 = 0;

em_sysmng_stg_t sys_setting = {0};
em_cmdsetting_t cmd_setting[] = {
	{"shutdown", &cmd_shutdown},
	{"pause", &cmd_pause}};

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
	b_shutdown = 1;

	em_cmd_stop(&sysmng.cmdmng);
}

void cmd_pause(int argc, char **argv)
{
	if (argc <= 2)
		return;

	if (atoi(argv[1]) == 1)
	{
		printf("==== app1 pause %s ! ====\n", argv[2]);
		b_pause1 = atoi(argv[2]);
	}

	if (atoi(argv[1]) == 2)
	{
		printf("==== app2 pause %s ! ====\n", argv[2]);
		b_pause2 = atoi(argv[2]);
	}
}

int app1_init(void *arg)
{
	em_printf(EM_LOG_INFO, "app1 init\n");
	return 0;
}

int app2_init(void *arg)
{
	// gevent = em_evtmng_factory(&sysmng.evtmng);
	em_printf(EM_LOG_INFO, "app2 init\n");
	return 0;
}

int app1_signal(int arg)
{
	b_shutdown1 = 1;
	em_printf(EM_LOG_INFO, "app1 signal\n");
	return 0;
}

int app2_signal(int arg)
{
	b_shutdown2 = 1;
	em_printf(EM_LOG_INFO, "app2 signal\n");
	return 0;
}

int app1_main()
{
	testmsg_t msg;
	timer_arg_msg_t timerarg = {TASK_ID_APP1, 10};
	uint timer_id;

	em_timersetting_t timersetting = {1000, timer_func_msg, &timerarg};
	if (0 != em_timer_create(&sysmng.tmrmng, &timersetting, &timer_id))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
	em_printf(EM_LOG_INFO, "[App1] timer created\n");

	while (!b_shutdown1)
	{
		if (b_pause1)
		{
			sleep(1);
			continue;
		}
		em_msg_recv(&sysmng.tskmng, &msg, EM_NO_TIMEOUT);

		em_printf(EM_LOG_INFO, " => [App1] msgType=%d, data=%d\n", msg.msg_type, msg.data);
	}

	if (0 != em_timer_delete(&sysmng.tmrmng, timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	em_printf(EM_LOG_INFO, "[App1] timer deleted\n");

	return 0;
}

int app2_main()
{
	testmsg_t msg;
	timer_arg_evt_t timerarg = {EVENT_TIMER};
	uint timer_id;

	em_timersetting_t timersetting = {2500, timer_func_evt, &timerarg};
	if (0 != em_timer_create(&sysmng.tmrmng, &timersetting, &timer_id))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
	em_printf(EM_LOG_INFO, "[App2] timer created\n");

	while (!b_shutdown2)
	{
		if (b_pause2)
		{
			sleep(1);
			continue;
		}
		em_evtarray_wait(&sysmng.gevents, EVENT_TIMER, EM_NO_TIMEOUT);
		em_printf(EM_LOG_INFO, " => [App2] event received\n");

		memset(&msg, 0, sizeof(testmsg_t));
		msg.msg_type = 21;
		msg.data = 100;
		em_msg_send(&sysmng.tskmng, TASK_ID_APP1, &msg, 1000);
	}

	if (0 != em_timer_delete(&sysmng.tmrmng, timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	em_printf(EM_LOG_INFO, "[App2] timer deleted\n");

	return 1;
}

static int cmd_main()
{
	// shutdown command
	int cmd_num = sizeof(cmd_setting) / sizeof(em_cmdsetting_t);
	for (int i = 0; i < cmd_num; i++)
	{
		if (0 != em_cmd_regist(&sysmng.cmdmng, &cmd_setting[i]))
		{
			em_printf(EM_LOG_INFO, "error\n");
		}
	}
	em_cmd_start(&sysmng.cmdmng);

	return 1;
}

static void timer_func_msg(void *arg)
{
	timer_arg_msg_t *timer_arg = (timer_arg_msg_t *)arg;
	em_printf(EM_LOG_INFO, "timer_func_msg => \n");
	testmsg_t msg;
	msg.msg_type = timer_arg->data;
	msg.data = em_get_tick_count(&sysmng.timemng);
	if (0 != em_msg_send(&sysmng.tskmng, timer_arg->task_to, &msg, 1000))
	{
		em_printf(EM_LOG_ERROR, "msg send failed\n");
	}
}

static void timer_func_evt(void *arg)
{
	timer_arg_evt_t *timer_arg = (timer_arg_evt_t *)arg;
	em_printf(EM_LOG_INFO, "timer_func_evt =>\n");

	if (0 != em_evtarray_broadcast(&sysmng.gevents, timer_arg->event_no))
	{
		em_printf(EM_LOG_ERROR, "event broadcast failed\n");
	}
}

int init()
{
	int max_alloc_num = 1000;
	int mem_unit_size = 1024;
	int mem_block_num = 1024;

	sys_setting.max_num_mutex = 100;
	sys_setting.max_num_sem = 100;
	sys_setting.max_num_event = 10;
	sys_setting.max_num_timer = 2;
	sys_setting.max_num_cmd = 5;
	sys_setting.max_num_task = 3;
	sys_setting.size_msgdata = sizeof(testmsg_t);
	sys_setting.mem_block_size = mem_unit_size;
	sys_setting.mem_block_num = mem_block_num;
	sys_setting.mem_alloc_num = max_alloc_num;
	sys_setting.mem_static = NULL;
	sys_setting.num_global_event = EVENT_MAXNUM;
	sys_setting.alloc_func = &local_malloc;
	sys_setting.free_func = &local_free;

	em_print_is_timeprint(TRUE);

	if (0 != em_memmng_create(&memmng, mem_unit_size, mem_block_num, max_alloc_num, NULL))
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
	em_datamng_destroy(&dm);

	em_memmng_print(&memmng, TRUE);

	if (0 != em_memmng_destroy(&memmng))
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

	// task initialize
	int task_num = sizeof(systaskstg) / sizeof(em_systaskinfo_t);

	em_printf(EM_LOG_INFO, "task num = %d\n", task_num);

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
			em_printf(EM_LOG_ERROR, "create task error\n");
			exit(1);
		}
	}

	while (b_shutdown)
	{
		sleep(1);
	}

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

	em_printf(EM_LOG_TOP, "[Main] system halted\n");
	return 0;
}
