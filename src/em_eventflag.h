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
#ifndef __EM_EVENTFLAG_H__
#define __EM_EVENTFLAG_H__

#include <pthread.h>
#include "em_cmndefs.h"
#include "em_mempool.h"

#define EM_NO_TIMEOUT -1
#define EM_NO_WAIT 0

typedef struct
{
	pthread_cond_t cond;
	pthread_mutex_t mtx;
} em_event_t;

int em_event_init(em_event_t *event);

int em_event_destroy(em_event_t *event);

int em_event_wait(em_event_t *event,
				  int timeout_ms);

int em_event_broadcast(em_event_t *event);

int em_event_set(em_event_t *event);

typedef struct
{
	uint array_size;
	em_event_t *events;
	void (*free_func)(void *);
} em_evtarray_t;

int em_evtarray_init(em_evtarray_t *evtarray,
					 uint array_size,
					 void *(*alloc_func)(size_t),
					 void (*free_func)(void *));

int em_evtarray_destroy(em_evtarray_t *evtarray);

int em_evtarray_wait(em_evtarray_t *evtarray,
					 uint event_id,
					 int timeout_ms);

int em_evtarray_broadcast(em_evtarray_t *evtarray,
						  uint event_id);

int em_evtarray_set(em_evtarray_t *evtarray,
					uint event_id);

typedef struct
{
	em_mpool_t mp_event;
	void (*free_func)(void *);
} em_evtmng_t;

int em_evtmng_init(em_evtmng_t *evtmng,
				   uint max_event_num,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_evtmng_destroy(em_evtmng_t *evtmng);

em_event_t *em_evtmng_factory(em_evtmng_t *smm);

int em_evtmng_dispose(em_evtmng_t *evtmng,
					  em_event_t *evt_p);

int em_evtmng_evtp2id(em_evtmng_t *evt,
					  em_event_t *evt_p);

em_event_t *em_evtmng_evtid2p(em_evtmng_t *evtmng,
							  int event_id);

#endif //__EM_EVENTFLAG_H__
