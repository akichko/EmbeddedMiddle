#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <time.h>

#include "../src/em_timer.h"

void timer_func(union sigval arg)
{
	static struct timespec curTime, lastTime;

	curTime = em_get_timestamp();
	if (lastTime.tv_sec == 0)
	{
		printf("Current  = ");
		em_print_timespec(curTime);
	}
	else
	{
		printf("Interval = ");
		em_print_timespec(em_timespec_sub(curTime, lastTime));
	}
	lastTime = curTime;
}

int main()
{
	timer_t timer_id;
	int ret;

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
