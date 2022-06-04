#ifndef __EM_TIMER_H__
#define __EM_TIMER_H__

#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "em_datamng.h"
//#include "em_task.h"

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
	// em_msg_t msg;
	// void (*timer_func)(void *);
	// void *arg;
} em_timerinfo_t;

typedef struct
{
	em_datamng_t timerinfo_mng;
} em_timermng_t;

int em_timer_create(timer_t *timer_id,
					void (*cb_function)(__sigval_t),
					int interval_ms);

int em_timer_delete(timer_t timer_id);

int em_timermng_init(em_timermng_t *tmrmng,
					 int num_timer);

int em_timer_create2(em_timermng_t *tmrmng,
					 em_timersetting_t *setting);

int em_timer_delete2(em_timermng_t *tmrmng,
					int timer_id);

#endif //__EM_TIMER_H__
