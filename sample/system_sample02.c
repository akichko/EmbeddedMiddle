#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "em_system.h"
#include "em_mqttc.h"
#include "em_geojson.h"

#define TASK_ID_APP1 100
#define TASK_ID_APP2 200
#define TASK_ID_CMD 300

typedef enum _event_no
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

typedef struct
{
	int task_to;
	int data;
} timer_arg_msg_t;

typedef struct
{
	e_event_no event_no;
} timer_arg_evt_t;

typedef struct
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
int cmd_main();
void timer_func_msg(void *arg);
void timer_func_evt(void *arg);
void cmd_shutdown(int argc, char **argv);

em_memmng_t memmng;
em_sysmng_t sysmng;
em_datamng_t dm;

em_systaskinfo_t systaskstg[] = {
	{{"App1", TASK_ID_APP1, 0, 0, 256, app1_main, TRUE}, &app1_init, &app1_signal},
	{{"App2", TASK_ID_APP2, 0, 0, 256, app2_main, TRUE}, &app2_init, &app2_signal},
	{{"Cmd", TASK_ID_CMD, 0, 0, 5, cmd_main, FALSE}, NULL, NULL}};

int b_shutdown1 = 0;
int b_shutdown2 = 0;
int b_shutdown = 0;

em_sysmng_stg_t sys_setting = {0};
em_cmdsetting_t shutdowncmd_setting = {"shutdown", &cmd_shutdown};

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

void subscribe_callback1(em_mqbuf_t *mqbuf)
{
	int mid;
	if (mqbuf->mid == NULL)
		mid = -1;
	else
		mid = *mqbuf->mid;

	printf("subscribe_callback1: mid=%d, topic=%s, payload=%s\n",
		   mid, mqbuf->topic, mqbuf->payload);
}

double app2_lon = 139.6217787;
double app2_lat = 35.4691287;
double app2_lon_diff = 0;
double app2_lat_diff = 0;

void subscribe_callback2(em_mqbuf_t *mqbuf)
{
	int mid;
	if (mqbuf->mid == NULL)
		mid = -1;
	else
		mid = *mqbuf->mid;

	printf("subscribe_callback2: mid=%d, topic=%s, payload=%s\n",
		   mid, mqbuf->topic, mqbuf->payload);

	if (0 == strcmp(mqbuf->topic, "sys1/app2/api/location"))
	{
		em_parse_geojson_point(&app2_lon, &app2_lat, mqbuf->payload);
		app2_lon_diff = 0;
		app2_lat_diff = 0;
	}
	else if (0 == strcmp(mqbuf->topic, "sys1/app2/api/command"))
	{
		em_cmd_exec(&sysmng.cmdmng, mqbuf->payload);
	}
}

int app1_main()
{
	em_jelem_t elem_id = {"id", EM_JSON_INT, {.integer = 1}};
	em_jelem_t elem_name = {"name", EM_JSON_STRING, {.string = "app1"}};
	em_jelem_t *elems[2] = {&elem_id, &elem_name};
	em_jobj_t property = {2, elems};

	double lon = 139.6217787;
	double lat = 35.4691287;
	double lon_diff = 0;
	double lat_diff = 0;

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

	em_mqttc_t mc;
	if (0 != em_mqttc_create(&mc, "client1", "localhost", 1883, NULL, NULL, NULL, 60, &malloc, &free))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_create error\n");
	}
	if (0 != em_mqttc_connect(&mc,10000))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_connect error\n");
		em_timer_delete(&sysmng.tmrmng, timer_id);
		return -1;
	}
	if (0 != em_mqttc_subscribe(&mc, "sys1/app1/api/command", &subscribe_callback1))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe_start error\n");
		em_timer_delete(&sysmng.tmrmng, timer_id);
		return -1;
	}

	while (!b_shutdown1)
	{
		em_msg_recv(&sysmng.tskmng, &msg, EM_NO_TIMEOUT);

		lon_diff += 0.001;
		if (lon_diff >= 0.01)
		{
			lon_diff = 0;
		}

		em_gjpoint_t geopoint = {lon + lon_diff, lat};
		em_gjgeometry_t geometry = {EM_GJ_POINT, {&geopoint}};

		em_gjfeature_t feature = {&geometry, &property};

		char json_str[256];
		em_geojson_snprint_feature(json_str, 256, &feature);

		if (0 != em_mqttc_publish_txt(&mc, NULL, "sys1/app1/upload/location", json_str))
		{
			em_printf(EM_LOG_ERROR, "em_mqttc_publish_txt error\n");
		}
		em_printf(EM_LOG_INFO, "app1 published.\n");
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
	em_jelem_t elem_id = {"id", EM_JSON_INT, {.integer = 2}};
	em_jelem_t elem_name = {"name", EM_JSON_STRING, {.string = "app2"}};
	em_jelem_t *elems[2] = {&elem_id, &elem_name};
	em_jobj_t property = {2, elems};

	testmsg_t msg;
	timer_arg_evt_t timerarg = {EVENT_TIMER};
	uint timer_id;

	em_timersetting_t timersetting = {2500, timer_func_evt, &timerarg};
	if (0 != em_timer_create(&sysmng.tmrmng, &timersetting, &timer_id))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
	em_printf(EM_LOG_INFO, "[App2] event timer created\n");

	em_mqttc_t mc;
	if (0 != em_mqttc_create(&mc, "client2", "localhost", 1883, NULL, NULL, NULL, 60, &malloc, &free))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_create error\n");
	}
	if (0 != em_mqttc_connect(&mc, 10000))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_connect error\n");
		em_timer_delete(&sysmng.tmrmng, timer_id);
		return -1;
	}
	if (0 != em_mqttc_subscribe(&mc, "sys1/app2/api/+", &subscribe_callback2))
	{
		em_printf(EM_LOG_ERROR, "em_mqttc_subscribe_start error\n");
		em_timer_delete(&sysmng.tmrmng, timer_id);
		return -1;
	}

	while (!b_shutdown2)
	{
		em_evtarray_wait(&sysmng.gevents, EVENT_TIMER, EM_NO_TIMEOUT);
		// em_printf(EM_LOG_INFO, " => [App2] event received\n");

		app2_lon_diff += 0.001;
		if (app2_lon_diff >= 0.01)
		{
			app2_lon_diff = 0;
		}

		em_gjpoint_t geopoint = {app2_lon + app2_lon_diff, app2_lat};
		em_gjgeometry_t geometry = {EM_GJ_POINT, {&geopoint}};

		em_gjfeature_t feature = {&geometry, &property};

		char json_str[256];
		em_geojson_snprint_feature(json_str, 256, &feature);

		if (0 != em_mqttc_publish_txt(&mc, NULL, "sys1/app2/upload/location", json_str))
		{
			em_printf(EM_LOG_ERROR, "em_mqttc_publish_txt error\n");
		}
		em_printf(EM_LOG_INFO, "app2 published.\n");
	}

	if (0 != em_timer_delete(&sysmng.tmrmng, timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	em_printf(EM_LOG_INFO, "[App2] timer deleted\n");

	return 1;
}

int cmd_main()
{
	// shutdown command
	if (0 != em_cmd_register(&sysmng.cmdmng, &shutdowncmd_setting))
	{
		em_printf(EM_LOG_INFO, "error\n");
	}

	em_cmd_start(&sysmng.cmdmng);

	return 1;
}

void timer_func_msg(void *arg)
{
	timer_arg_msg_t *timer_arg = (timer_arg_msg_t *)arg;
	// em_printf(EM_LOG_INFO, "timer_func_msg => \n");
	testmsg_t msg;
	msg.msg_type = timer_arg->data;
	msg.data = em_get_tick_count(&sysmng.timemng);
	if (0 != em_msg_send(&sysmng.tskmng, timer_arg->task_to, &msg, 1000))
	{
		em_printf(EM_LOG_ERROR, "msg send failed\n");
	}
}

void timer_func_evt(void *arg)
{
	timer_arg_evt_t *timer_arg = (timer_arg_evt_t *)arg;
	// em_printf(EM_LOG_INFO, "timer_func_evt =>\n");

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

	em_idatamng_create(&dm, 128, 128, EM_DMNG_DPLCT_ERROR, &local_malloc, &local_free);

	em_memmng_print(&memmng, TRUE);

	em_mqtt_lib_init();

	return 0;
}

int finalize()
{
	if (0 != em_sysmng_finalize(&sysmng))
	{
		printf("sys init error\n");
		return -1;
	}
	em_idatamng_destroy(&dm);

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

	while (!b_shutdown)
	{
		sleep(1);
	}

	for (int i = 0; i < task_num; i++)
	{
		if (systaskstg[i].task_stg.wait_shutdown == FALSE)
			continue;

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
