#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <time.h>

#include "../src/em_timer.h"

em_timemng_t tm;

void timer_func(union sigval arg)
{
	static struct timespec curTime, lastTime;
	int tick;

	curTime = em_get_timestamp();
	tick =  em_get_tick_count(&tm);

	if (lastTime.tv_sec == 0)
	{
		printf("[%6d] Current  = ", tick);
		em_print_timespec(curTime);
	}
	else
	{
		printf("[%6d] Interval = ", tick);
		em_print_timespec(em_timespec_sub(curTime, lastTime));
	}
	lastTime = curTime;
}

int main()
{
	timer_t timer_id;
	int ret;
	em_tick_init(&tm);

	ret = em_timer_create(&timer_id, timer_func, 1200);
	if (ret != 0)
	{
		printf("em_timer_create error!!\n");
		exit(1);
	}
	printf("timer created\n");

	sleep(7);

	ret = em_timer_delete(timer_id);
	if (ret != 0)
	{
		printf("em_timer_delete error!!\n");
		exit(1);
	}
	printf("timer deleted\n");

	return 0;
}
