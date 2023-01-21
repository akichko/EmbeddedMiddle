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
#ifndef __EM_TIMER_H__
#define __EM_TIMER_H__

#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "em_datamng.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct
{
	int interval_ms;
	void (*timer_func)(void *);
	void *arg;
} em_timersetting_t;

typedef struct
{
	timer_t linux_timer_id;
	em_timersetting_t setting;
} em_timerinfo_t;

typedef struct
{
	em_mpool_t mp_timerinfo;
} em_timermng_t;

int em_timermng_init(em_timermng_t *tmrmng,
						int num_timer,
						void *(*alloc_func)(size_t),
						void (*free_func)(void *));

int em_timermng_destroy(em_timermng_t *tmrmng);

int em_timer_create(em_timermng_t *tmrmng,
					em_timersetting_t *setting,
					uint *time_id);

int em_timer_delete(em_timermng_t *tmrmng,
					uint timer_id);

int em_timer_set_interval(em_timermng_t *tmrmng,
							uint timer_id,
							int interval_ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_TIMER_H__
