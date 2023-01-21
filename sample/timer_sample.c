#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <time.h>

#include "../src/em_time.h"
#include "../src/em_timer.h"

em_timemng_t tm;

void timer_func_2(void *arg)
{
	printf("func_2 [tid=%ld][%4d] %s\n", pthread_self(), em_get_tick_count(&tm), (char *)arg);
}

void timer_func_1(void *arg)
{
	static struct timespec curTime, lastTime;
	int tick;

	curTime = em_get_timestamp();
	tick = em_get_tick_count(&tm);

	if (lastTime.tv_sec == 0)
	{
		printf("func_1 [tid=%ld][%4d] Current  = ", pthread_self(), tick);
		em_print_timespec(curTime);
	}
	else
	{
		printf("func_1 [tid=%ld][%4d] Interval = ", pthread_self(), tick);
		em_print_timespec(em_timespec_sub(curTime, lastTime));
	}
	lastTime = curTime;
}

char teststr[] = "test string";

em_timersetting_t timersetting[] = {
	{1200, timer_func_1, NULL},
	//	{200, 900, timer_func_1, NULL},
	{900, timer_func_2, teststr}};

int main()
{
	em_timermng_t tmrmng;
	em_tick_init(&tm);

	int timer_num = sizeof(timersetting) / sizeof(em_timersetting_t);
	uint timer_id[timer_num];

	if (0 != em_timermng_init(&tmrmng, 10, &malloc, &free))
	{
		printf("error\n");
	}

	for (int i = 0; i < timer_num; i++)
	{
		if (0 != em_timer_create(&tmrmng, &timersetting[i], &timer_id[i]))
		{
			printf("em_timer_create error!!\n");
			exit(1);
		}
	}

	sleep(7);

	if (0 != em_timer_set_interval(&tmrmng, timer_id[0], 300))
	{
		printf("em_timer_set_interval error!!\n");
		exit(1);
	}

	sleep(3);

	for (int i = 0; i < timer_num; i++)
	{
		if (0 != em_timer_delete(&tmrmng, timer_id[i]))
		{
			printf("em_timer_delete error!!\n");
			exit(1);
		}
	}

	return 0;
}
