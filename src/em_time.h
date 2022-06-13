#ifndef __EM_TIME_H__
#define __EM_TIME_H__

#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "em_datamng.h"
//#include "em_task.h"

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

#endif //__EM_TIME_H__
