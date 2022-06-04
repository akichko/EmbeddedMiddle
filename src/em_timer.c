#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <time.h>
#include "em_time.h"
#include "em_timer.h"
#include "em_datamng.h"

int em_timermng_init(em_timermng_t *tmrmng, int num_timer)
{
	return em_datamng_create(&tmrmng->timerinfo_mng, sizeof(em_timerinfo_t), num_timer);
}

void _em_timer_cbfunc(__sigval_t sigval)
{
	em_timersetting_t *timersetting = (em_timersetting_t *)sigval.sival_ptr;

	timersetting->timer_func(timersetting->arg);
}

int em_timer_create(em_timermng_t *tmrmng, em_timersetting_t *setting)
{
	timer_t timer_id;
	int ret;

	struct sigevent se;
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_notify_function = _em_timer_cbfunc;
	se.sigev_value.sival_ptr = setting;
	se.sigev_notify_attributes = NULL;

	struct itimerspec ts;
	ts.it_value = em_calc_timespec(setting->interval_ms);
	ts.it_interval = em_calc_timespec(setting->interval_ms);

	ret = timer_create(CLOCK_MONOTONIC, &se, &timer_id);
	if (ret == -1)
	{
		printf("Fail to creat timer\n");
		return -1;
	}

	em_timerinfo_t timerinfo;
	timerinfo.linux_timer_id = timer_id;
	ret = em_datamng_add_data(&tmrmng->timerinfo_mng, setting->timer_id, &timerinfo);
	if (ret == -1)
	{
		printf("Error: em_datamng_add_data\n");
		return -2;
	}

	ret = timer_settime(timer_id, 0, &ts, 0);
	if (ret == -1)
	{
		printf("Fail to set timer\n");
		return -2;
	}

	printf("timer created. id=%d\n", setting->timer_id);
	return 0;
}

int em_timer_delete(em_timermng_t *tmrmng, int timer_id)
{
	int ret;
	em_timerinfo_t timer_info;

	if (0 != em_datamng_get_data(&tmrmng->timerinfo_mng, timer_id, (void *)&timer_info))
	{
		printf("error\n");
		return -1;
	}
	ret = timer_delete(timer_info.linux_timer_id);
	if (ret != 0)
	{
		printf("timer delete error!!\n");
	}

	return ret;
}
