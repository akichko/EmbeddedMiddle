#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/timerfd.h>
#include <time.h>
#include "em_time.h"
#include "em_datamng.h"
#include "em_print.h"

struct timespec em_get_timestamp()
{
	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		em_printf(EM_LOG_ERROR, "Error: clock_gettime\n");
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
	em_printf(EM_LOG_ERROR, "%10ld.%09ld\n", ts.tv_sec, ts.tv_nsec);
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
		em_printf(EM_LOG_ERROR, "Error: em_tick_init\n");
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
