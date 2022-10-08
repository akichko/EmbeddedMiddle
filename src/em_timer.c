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
#include <string.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <time.h>
#include "em_time.h"
#include "em_timer.h"
#include "em_datamng.h"

int em_timermng_init(em_timermng_t *tmrmng, int num_timer, void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	int ret = em_mpool_create(&tmrmng->mp_timerinfo, sizeof(em_timerinfo_t), num_timer,
							  alloc_func, free_func);
	return ret;
}

int em_timermng_destroy(em_timermng_t *tmrmng)
{
	int ret = em_mpool_delete(&tmrmng->mp_timerinfo);
	return ret;
}

static void _em_timer_cbfunc(__sigval_t sigval)
{
	em_timersetting_t *timersetting = (em_timersetting_t *)sigval.sival_ptr;

	timersetting->timer_func(timersetting->arg);
}

int em_timer_create(em_timermng_t *tmrmng, em_timersetting_t *setting, uint *timer_id)
{
	timer_t linux_timer_id;
	int ret;

	em_timerinfo_t *timer_info;
	if (0 != em_mpool_alloc_block(&tmrmng->mp_timerinfo, (void **)&timer_info, EM_NO_WAIT))
	{
		em_printf(EM_LOG_ERROR, "mem alloc error\n");
		return -1;
	}

	memcpy(&timer_info->setting, setting, sizeof(em_timersetting_t));

	*timer_id = 1 + em_mpool_get_dataidx(&tmrmng->mp_timerinfo, timer_info);

	struct sigevent se;
	se.sigev_notify = SIGEV_THREAD;
	se.sigev_notify_function = _em_timer_cbfunc;
	se.sigev_value.sival_ptr = &timer_info->setting; // heap or static
	se.sigev_notify_attributes = NULL;

	ret = timer_create(CLOCK_MONOTONIC, &se, &linux_timer_id);
	if (ret == -1)
	{
		em_printf(EM_LOG_ERROR, "Fail to creat timer\n");
		em_mpool_free_block(&tmrmng->mp_timerinfo, timer_info);

		return -1;
	}

	timer_info->linux_timer_id = linux_timer_id;

	struct itimerspec ts;
	ts.it_value = em_calc_timespec(setting->interval_ms);
	ts.it_interval = em_calc_timespec(setting->interval_ms);

	ret = timer_settime(linux_timer_id, 0, &ts, 0);
	if (ret == -1)
	{
		em_printf(EM_LOG_ERROR, "Fail to set timer\n");
		em_mpool_free_block(&tmrmng->mp_timerinfo, timer_info);
		return -2;
	}

	em_printf(EM_LOG_INFO, "timer created. id=%d %p\n", *timer_id, linux_timer_id);
	return 0;
}

int em_timer_delete(em_timermng_t *tmrmng, uint timer_id)
{
	int ret;
	em_timerinfo_t *timer_info = (em_timerinfo_t*)em_mpool_get_dataptr(&tmrmng->mp_timerinfo, timer_id - 1);

	if (timer_info == NULL)
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}

	ret = timer_delete(timer_info->linux_timer_id);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "timer delete error!!\n");
	}
	em_mpool_free_block(&tmrmng->mp_timerinfo, timer_info);
	em_printf(EM_LOG_INFO, "timer deleted. id=%d %p\n", timer_id, timer_info->linux_timer_id);

	return ret;
}
