#ifndef __EM_TIMER_H__
#define __EM_TIMER_H__

#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "em_datamng.h"

typedef struct
{
	int timer_id;
	int interval_ms;
	void (*timer_func)(void *);
	void *arg;
} em_timersetting_t;

typedef struct
{
	timer_t linux_timer_id;
} em_timerinfo_t;

typedef struct
{
	em_datamng_t timerinfo_mng;
} em_timermng_t;

int em_timermng_init(em_timermng_t *tmrmng,
					 int num_timer);

int em_timer_create(em_timermng_t *tmrmng,
					 em_timersetting_t *setting);

int em_timer_delete(em_timermng_t *tmrmng,
					int timer_id);

#endif //__EM_TIMER_H__
