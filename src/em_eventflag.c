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
			return EM_ERR_TIMEOUT;
		}
	}
	return EM_SUCCESS;
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

// event array

int em_evtarray_init(em_evtarray_t *evtarray, uint array_size,
					 void *(*alloc_func)(size_t),
					 void (*free_func)(void *))
{
	evtarray->array_size = array_size;
	evtarray->free_func = free_func;
	evtarray->events = (em_event_t *)alloc_func(sizeof(em_event_t) * array_size);
	for (uint i = 0; i < array_size; i++)
	{
		int ret = em_event_init(&evtarray->events[i]);
		if (ret != 0)
		{
			em_printf(EM_LOG_ERROR, "init error\n");
			return -1;
		}
	}
	return 0;
}

int em_evtarray_destroy(em_evtarray_t *evtarray)
{

	for (uint i = 0; i < evtarray->array_size; i++)
	{
		int ret = em_event_destroy(&evtarray->events[i]);
		if (ret != 0)
		{
			em_printf(EM_LOG_ERROR, "destroy error\n");
			return -1;
		}
	}
	evtarray->free_func(evtarray->events);
	return 0;
}

int em_evtarray_wait(em_evtarray_t *evtarray, uint event_id, int timeout_ms)
{
	if (event_id >= evtarray->array_size)
	{
		em_printf(EM_LOG_ERROR, "param error\n");
	}
	return em_event_wait(&evtarray->events[event_id], timeout_ms);
}

int em_evtarray_broadcast(em_evtarray_t *evtarray, uint event_id)
{
	if (event_id >= evtarray->array_size)
	{
		em_printf(EM_LOG_ERROR, "param error\n");
	}
	return em_event_broadcast(&evtarray->events[event_id]);
}

int em_evtarray_set(em_evtarray_t *evtarray, uint event_id)
{
	if (event_id >= evtarray->array_size)
	{
		em_printf(EM_LOG_ERROR, "param error\n");
	}
	return em_event_set(&evtarray->events[event_id]);
}

// event manage

int em_evtmng_init(em_evtmng_t *evtmng, uint max_event_num,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *))
{
	evtmng->free_func = free_func;
	if (0 != em_mpool_create(&evtmng->mp_event, sizeof(em_event_t), max_event_num, alloc_func, free_func))
	{
		em_printf(EM_LOG_ERROR, "init error\n");
		return -1;
	}

	return 0;
}

int em_evtmng_destroy(em_evtmng_t *evtmng)
{
	int ret = em_mpool_destroy(&evtmng->mp_event);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	return 0;
}

em_event_t *em_evtmng_factory(em_evtmng_t *evtmng)
{
	em_event_t *event;
	if (0 != em_mpool_alloc_block(&evtmng->mp_event, (void **)&event, EM_NO_WAIT))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return NULL;
	}
	if (0 != em_event_init(event))
	{
		return NULL;
	}
	return event;
}

int em_evtmng_dispose(em_evtmng_t *evtmng, em_event_t *evt_p)
{
	if (0 != em_event_destroy((em_event_t *)evt_p))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	return em_mpool_free_block(&evtmng->mp_event, evt_p);
}

int em_evtmng_evtp2id(em_evtmng_t *evtmng, em_event_t *evt_p)
{
	return em_mpool_get_dataidx(&evtmng->mp_event, evt_p);
}

em_event_t *em_evtmng_evtid2p(em_evtmng_t *evtmng, int event_id)
{
	return (em_event_t *)em_mpool_get_dataptr(&evtmng->mp_event, event_id);
}
