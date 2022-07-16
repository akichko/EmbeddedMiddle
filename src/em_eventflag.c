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
	pthread_cond_init(&event->cond, NULL);
	pthread_mutex_init(&event->mtx, NULL);
}

int em_event_destroy(em_event_t *event)
{
	pthread_cond_destroy(&event->cond);
	pthread_mutex_destroy(&event->mtx);
}

int em_event_wait(em_event_t *event, int timeout_ms)
{
	int ret;
	if (timeout_ms == EM_NO_TIMEOUT)
	{
		pthread_mutex_lock(&event->mtx);
		ret = pthread_cond_wait(&event->cond, &event->mtx);
		pthread_mutex_unlock(&event->mtx);
	}
	else
	{
		struct timespec timeout_ts = em_get_offset_timestamp(timeout_ms);
		pthread_mutex_lock(&event->mtx);
		ret = pthread_cond_timedwait(&event->cond, &event->mtx, &timeout_ts);
		pthread_mutex_unlock(&event->mtx);
	}
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "wait error\n");
	}
	return ret;
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
