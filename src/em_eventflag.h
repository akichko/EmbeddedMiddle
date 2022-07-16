#ifndef __EM_EVENTFLAG_H__
#define __EM_EVENTFLAG_H__

#include <pthread.h>

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

#endif //__EM_EVENTFLAG_H__
