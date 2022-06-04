#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <time.h>
#include "em_timer.h"
#include "em_datamng.h"

struct timespec em_get_timestamp()
{
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		printf("Error: clock_gettime\n");
	return ts;
}

struct timespec em_timespec_add(struct timespec a, struct timespec b)
{
	struct timespec ret = a;

	ret.tv_sec += b.tv_sec;
	ret.tv_nsec += b.tv_nsec;
	if (ret.tv_nsec >= 1000000000)
	{
		ret.tv_sec += 1;
		ret.tv_nsec -= 1000000000;
	}

	return ret;
}

struct timespec em_timespec_sub(struct timespec a, struct timespec b)
{
	struct timespec ret = a;

	ret.tv_sec -= b.tv_sec;
	ret.tv_nsec -= b.tv_nsec;
	if (ret.tv_nsec < 0)
	{
		ret.tv_sec -= 1;
		ret.tv_nsec += 1000000000;
	}

	return ret;
}

void em_print_timespec(struct timespec ts)
{
	printf("%10ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
}

struct timespec em_get_offset_timestamp(int milliseconds)
{
	struct timespec ts = em_get_timestamp();

	ts.tv_sec += (milliseconds / 1000);
	ts.tv_nsec += (milliseconds % 1000) * 1000000;
	if (ts.tv_nsec >= 1000000000)
	{
		ts.tv_sec += 1;
		ts.tv_nsec -= 1000000000;
	}

	return ts;
}

struct timespec em_calc_timespec(int milliseconds)
{
	struct timespec ts;
	ts.tv_sec = (milliseconds / 1000);
	ts.tv_nsec = (milliseconds % 1000) * 1000000;

	return ts;
}

struct timeval em_calc_timeval(int milliseconds)
{
	struct timeval tv;
	tv.tv_sec = (milliseconds / 1000);
	tv.tv_usec = (milliseconds % 1000) * 1000;

	return tv;
}

int em_tick_init(em_timemng_t *tm)
{
	struct timespec ts;
	tm->is_initialized = 0;

	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
	{
		printf("Error: em_tick_init\n");
		return -1;
	}
	tm->init_time = ts;
	tm->is_initialized = 1;
	return 0;
}

int em_get_tick_count(em_timemng_t *tm)
{
	if (tm->is_initialized == 0)
	{
		return -1;
	}

	struct timespec current_ts = em_get_timestamp();
	struct timespec ret = em_timespec_sub(current_ts, tm->init_time);

	return ret.tv_sec * 1000 + ret.tv_nsec / 1000000;
}

int em_timer_create(timer_t *timer_id, void (*cb_function)(__sigval_t), int interval_ms)
{
	// static int no = 1;
	struct sigevent se;
	struct itimerspec ts;
	int ret;

	// se.sigev_signo = no++;
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_notify_function = cb_function;
	se.sigev_value.sival_ptr = timer_id;
	se.sigev_notify_attributes = NULL;

	ts.it_value = em_calc_timespec(interval_ms);
	ts.it_interval = em_calc_timespec(interval_ms);

	ret = timer_create(CLOCK_MONOTONIC, &se, timer_id);
	if (ret == -1)
	{
		printf("Fail to creat timer\n");
		return -1;
	}

	ret = timer_settime(*timer_id, 0, &ts, 0);
	if (ret == -1)
	{
		printf("Fail to set timer\n");
		return -2;
	}

	return 0;
}

int em_timermng_init(em_timermng_t *tmrmng, int num_timer)
{
	return em_datamng_create(&tmrmng->timerinfo_mng, sizeof(em_timerinfo_t), num_timer);
}

void _em_timer_cbfunc(__sigval_t sigval)
{
	em_timersetting_t *timersetting = (em_timersetting_t *)sigval.sival_ptr;

	timersetting->timer_func(timersetting->arg);
}

int em_timer_create2(em_timermng_t *tmrmng, em_timersetting_t *setting)
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

	printf("timer created. id=%p\n", timer_id);
	return 0;
}

int em_timer_delete(timer_t timer_id)
{
	int ret;

	ret = timer_delete(timer_id);
	if (ret != 0)
	{
		printf("timer delete error!!\n");
	}

	return ret;
}

int em_timer_delete2(em_timermng_t *tmrmng, int timer_id)
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
