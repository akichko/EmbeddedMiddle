#ifndef __EM_TIMER_H__
#define __EM_TIMER_H__

#include <signal.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <time.h>


int em_timer_create(timer_t *timer_id, void (*cb_function)(__sigval_t), int interval_ms);

int em_timer_delete(timer_t timer_id);


#endif //__EM_TIMER_H__
