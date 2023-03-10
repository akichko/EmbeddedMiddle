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

// event

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

// event flag

int em_eventflag_init(em_eventflg_t *ef)
{
	if (ef == NULL)
		return EM_ERR_PARAM;

	ef->flags = 0;

	if (pthread_cond_init(&ef->cond, NULL) != 0)
	{
		em_printf(EM_LOG_ERROR, "cond init error\n");
		return EM_ERR_SYS;
	}
	if (pthread_mutex_init(&ef->mtx, NULL) != 0)
	{
		em_printf(EM_LOG_ERROR, "mutex init error\n");
		return EM_ERR_SYS;
	}

	return EM_SUCCESS;
}

int em_eventflag_destroy(em_eventflg_t *ef)
{
	if (ef == NULL)
		return EM_ERR_PARAM;

	if (pthread_cond_destroy(&ef->cond) != 0)
	{
		em_printf(EM_LOG_ERROR, "cond destroy error\n");
		return EM_ERR_SYS;
	}
	if (pthread_mutex_destroy(&ef->mtx) != 0)
	{
		em_printf(EM_LOG_ERROR, "mutex destroy error\n");
		return EM_ERR_SYS;
	}

	return EM_SUCCESS;
}

int em_eventflag_wait(em_eventflg_t *ef, uint flags, int timeout_ms, int reset_flag)
{
	if (ef == NULL)
		return EM_ERR_PARAM;

	int ret = 0;
	struct timespec timeout_ts;

	if (timeout_ms != EM_NO_TIMEOUT)
		timeout_ts = em_get_offset_timestamp(timeout_ms);

	pthread_mutex_lock(&ef->mtx); // acquire the lock for thread safety

	// loop until the desired flag is set or the timeout expires
	while ((ef->flags & flags) == 0)
	{
		if (timeout_ms == EM_NO_TIMEOUT)
		{
			// wait indefinitely until the flag is set
			ret = pthread_cond_wait(&ef->cond, &ef->mtx);
			if (ret != 0)
			{
				ret = EM_ERROR;
				break;
			}
		}
		else
		{
			// wait for the specified timeout period
			ret = pthread_cond_timedwait(&ef->cond, &ef->mtx, &timeout_ts);
			if (ret == ETIMEDOUT)
			{
				ret = EM_ERR_TIMEOUT;
				break;
			}
			else if (ret != 0)
			{
				ret = EM_ERROR;
				break;
			}
		}
	}

	if (ret == 0)
		ret = EM_SUCCESS;

	pthread_mutex_unlock(&ef->mtx); // release the lock

	if (ret == EM_SUCCESS && reset_flag)
	{
		pthread_mutex_lock(&ef->mtx); // acquire the lock for thread safety
		ef->flags &= ~flags;
		pthread_mutex_unlock(&ef->mtx); // release the lock
	}

	return ret;
}

int em_eventflag_set(em_eventflg_t *ef, uint flags)
{
	if (ef == NULL)
		return EM_ERR_PARAM;

	pthread_mutex_lock(&ef->mtx); // acquire the lock for thread safety

	ef->flags |= flags;				   // set the desired flag
	pthread_cond_broadcast(&ef->cond); // wake up all waiting threads

	pthread_mutex_unlock(&ef->mtx); // release the lock

	return EM_SUCCESS;
}

int em_eventflag_clear(em_eventflg_t *ef)
{
	if (ef == NULL)
		return EM_ERR_PARAM;

	pthread_mutex_lock(&ef->mtx);
	ef->flags = 0;
	pthread_mutex_unlock(&ef->mtx);

	return EM_SUCCESS;
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
