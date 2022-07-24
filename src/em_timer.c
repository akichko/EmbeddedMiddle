/*============================================================================
MIT License

Copyright (c) 2022 akichko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/
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
#include "em_print.h"

int em_timermng_init(em_timermng_t *tmrmng, int num_timer, void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	return em_datamng_create(&tmrmng->timerinfo_mng, sizeof(em_timerinfo_t), num_timer, EM_DMNG_DPLCT_ERROR, alloc_func, free_func);
}

int em_timermng_destroy(em_timermng_t *tmrmng)
{
	return em_datamng_delete(&tmrmng->timerinfo_mng);
}
static void _em_timer_cbfunc(__sigval_t sigval)
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
	se.sigev_value.sival_ptr = setting; // heap or static
	se.sigev_notify_attributes = NULL;

	struct itimerspec ts;
	ts.it_value = em_calc_timespec(setting->interval_ms);
	ts.it_interval = em_calc_timespec(setting->interval_ms);

	ret = timer_create(CLOCK_MONOTONIC, &se, &timer_id);
	if (ret == -1)
	{
		em_printf(EM_LOG_ERROR, "Fail to creat timer\n");
		return -1;
	}

	em_timerinfo_t timerinfo;
	timerinfo.linux_timer_id = timer_id;
	ret = em_datamng_add_data(&tmrmng->timerinfo_mng, setting->timer_id, &timerinfo);
	if (ret == -1)
	{
		em_printf(EM_LOG_ERROR, "Error: em_datamng_add_data\n");
		return -2;
	}

	ret = timer_settime(timer_id, 0, &ts, 0);
	if (ret == -1)
	{
		em_printf(EM_LOG_ERROR, "Fail to set timer\n");
		return -2;
	}

	em_printf(EM_LOG_INFO, "timer created. id=%d\n", setting->timer_id);
	return 0;
}

int em_timer_delete(em_timermng_t *tmrmng, int timer_id)
{
	int ret;
	em_timerinfo_t timer_info;

	if (0 != em_datamng_get_data(&tmrmng->timerinfo_mng, timer_id, (void *)&timer_info))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	ret = timer_delete(timer_info.linux_timer_id);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "timer delete error!!\n");
	}

	return ret;
}
