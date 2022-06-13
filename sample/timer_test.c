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

em_timersetting_t timersetting[] = {
	{100, 1200, timer_func_1, NULL},
	{200, 900, timer_func_1, NULL},
	{300, 900, timer_func_2, "test string"}};

int main()
{
	em_timermng_t tmrmng;
	// timer_t timer1_id, timer2_id;
	//int ret;
	em_tick_init(&tm);

	if (0 != em_timermng_init(&tmrmng, 10))
	{
		printf("error\n");
	}

	if (0 != em_timer_create(&tmrmng, &timersetting[0]))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
#if 0
	if (0 != em_timer_create(&tmrmng, &timersetting[1]))
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
#endif
	if (0 != em_timer_create(&tmrmng, &timersetting[2]))
	{
		printf("em_timer_create2 error!!\n");
		exit(1);
	}

	sleep(7);

	if (0 != em_timer_delete(&tmrmng, timersetting[0].timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	printf("timer1 deleted\n");
#if 0
	if (0 != em_timer_delete(&tmrmng, timersetting[1].timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	printf("timer2 deleted\n");
#endif
	if (0 != em_timer_delete(&tmrmng, timersetting[2].timer_id))
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	printf("timer3 deleted\n");

	return 0;
}
