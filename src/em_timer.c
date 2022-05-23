#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <time.h>
#include "em_timer.h"


int em_timer_create(timer_t *timer_id, void (*cb_function) (__sigval_t), int interval_ms)
{
	struct sigevent se;
	struct itimerspec ts;
	int status;
	int ret;

	se.sigev_notify = SIGEV_THREAD;
	se.sigev_notify_function = cb_function;
	se.sigev_value.sival_ptr = timer_id;
	se.sigev_notify_attributes = NULL;

	ts.it_value.tv_sec = (interval_ms / 1000);
	ts.it_value.tv_nsec = (interval_ms % 1000) * 1000000;
	ts.it_interval.tv_sec = interval_ms / 1000;
	ts.it_interval.tv_nsec = (interval_ms % 1000) * 1000000;

	printf("create timer\n");

	status = timer_create(CLOCK_MONOTONIC, &se, timer_id);
	if (status == -1)
	{
		printf("Fail to creat timer\n");
		return -1;
	}

	status = timer_settime(*timer_id, 0, &ts, 0);
	if (status == -1)
	{
		printf("Fail to set timer\n");
		return -2;
	}

	return 0;
}

int em_timer_delete(timer_t timer_id)
{
	int ret;

	ret = timer_delete(timer_id);
	if (ret != 0){
		printf("timer delete error!!\n");
	}else{
		printf("timer deleted\n");
	}
	return ret;
}

