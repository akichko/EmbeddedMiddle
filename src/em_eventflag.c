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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include "em_eventflag.h"
#include "em_time.h"

int em_event_init(em_event_t *event)
{
	if (0 != pthread_cond_init(&event->cond, NULL))
	{
		em_printf(EM_LOG_ERROR, "cond init error\n");
		return -1;
	}
	if (0 != pthread_mutex_init(&event->mtx, NULL))
	{
		em_printf(EM_LOG_ERROR, "mutex init error\n");
		return -1;
	}
	return 0;
}

int em_event_destroy(em_event_t *event)
{
	if (0 != pthread_cond_destroy(&event->cond))
	{
		em_printf(EM_LOG_ERROR, "cond init error\n");
		return -1;
	}
	if (0 != pthread_mutex_destroy(&event->mtx))
	{
		em_printf(EM_LOG_ERROR, "mutex init error\n");
		return -1;
	}
	return 0;
}

int em_event_wait(em_event_t *event, int timeout_ms)
{
	int ret;
	if (timeout_ms == EM_NO_TIMEOUT)
	{
		pthread_mutex_lock(&event->mtx);
		ret = pthread_cond_wait(&event->cond, &event->mtx);
		pthread_mutex_unlock(&event->mtx);
		if (ret != 0)
		{
			em_printf(EM_LOG_ERROR, "wait error\n");
			return EM_ERROR;
		}
	}
	else
	{
		struct timespec timeout_ts = em_get_offset_timestamp(timeout_ms);
		pthread_mutex_lock(&event->mtx);
		ret = pthread_cond_timedwait(&event->cond, &event->mtx, &timeout_ts);
		pthread_mutex_unlock(&event->mtx);
		if (ret != 0)
		{
			em_printf(EM_LOG_DEBUG, "wait timeout\n");
			return EM_E_TIMEOUT;
		}
	}
	return EM_ERROR;
}

int em_event_broadcast(em_event_t *event)
{
	if (0 != pthread_mutex_lock(&event->mtx))
	{
		em_printf(EM_LOG_ERROR, "lock error\n");
	}
	if (0 != pthread_cond_broadcast(&event->cond))
	{
		em_printf(EM_LOG_ERROR, "broadcast error\n");
	}
	if (0 != pthread_mutex_unlock(&event->mtx))
	{
		em_printf(EM_LOG_ERROR, "signal error\n");
	}
	return 0;
}

int em_event_set(em_event_t *event)
{
	if (0 != pthread_mutex_lock(&event->mtx))
	{
		em_printf(EM_LOG_ERROR, "lock error\n");
	}
	if (0 != pthread_cond_signal(&event->cond))
	{
		em_printf(EM_LOG_ERROR, "signal error\n");
	}
	if (0 != pthread_mutex_unlock(&event->mtx))
	{
		em_printf(EM_LOG_ERROR, "signal error\n");
	}

	return 0;
}
