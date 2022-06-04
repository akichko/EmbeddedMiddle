#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "em_mutex.h"
#include "em_timer.h"
#include "em_code.h"

int em_mutex_init(em_mutex_t *mutex)
{
	// mutex->mtx = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(&mutex->mtx, NULL);
}

int em_mutex_destroy(em_mutex_t *mutex)
{
	pthread_mutex_destroy(&mutex->mtx);
}

int em_mutex_lock(em_mutex_t *mutex, int timeout_ms)
{
	if (mutex == NULL)
	{
		return -1;
	}

	if (timeout_ms == EM_NO_TIMEOUT)
	{
		if (0 != pthread_mutex_lock(&mutex->mtx))
		{
			printf("Fatal Error\n");
			return -2;
		}
	}
	else
	{
		struct timespec ts = em_get_offset_timestamp(timeout_ms);

		if (0 != pthread_mutex_timedlock(&mutex->mtx, &ts))
		{
			return -1;
		}
	}
	return 0;
}

int em_mutex_unlock(em_mutex_t *mutex)
{
	if(0 != pthread_mutex_unlock(&mutex->mtx)){
		return -1;
	}

	return 0;
}
