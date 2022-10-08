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
#ifndef __EM_TIME_H__
#define __EM_TIME_H__

#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "em_datamng.h"
//#include "em_task.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct
{
	int is_initialized;
	struct timespec init_time;
} em_timemng_t;

struct timespec em_calc_timespec(int milliseconds);

struct timeval em_calc_timeval(int milliseconds);

struct timespec em_get_timestamp();

int em_tick_init(em_timemng_t *tm);

int em_get_tick_count(em_timemng_t *tm);

struct timespec em_get_offset_timestamp(int milliseconds);

int em_calc_timespec2ms(struct timespec ts);

struct timespec em_timespec_add(struct timespec a,
								struct timespec b);

struct timespec em_timespec_sub(struct timespec a,
								struct timespec b);

void em_print_timespec(struct timespec ts);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_TIME_H__
