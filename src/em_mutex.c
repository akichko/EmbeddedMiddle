#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "em_mutex.h"
#include "em_timer.h"

int em_mutex_init(em_mutex_t *mutex)
{
	mutex->mtx = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init((pthread_mutex_t *)mutex->mtx, NULL);
}

int em_mutex_destroy(em_mutex_t *mutex)
{
	pthread_mutex_destroy((pthread_mutex_t *)mutex->mtx);
}

int em_mutex_lock(em_mutex_t *mutex, int timeout_ms)
{
	int ret;
	if (timeout_ms == EM_NO_TIMEOUT)
	{
		pthread_mutex_lock((pthread_mutex_t *)mutex->mtx);
	}
	else
	{
		struct timespec ts = em_get_offset_timestamp(timeout_ms);

		ret = pthread_mutex_timedlock((pthread_mutex_t *)mutex->mtx, &ts);
		if (ret != 0)
		{
			// printf("Error: em_mutex_lock\n");
			return -1;
		}
	}
	return 0;
}

int em_mutex_unlock(em_mutex_t *mutex)
{
	pthread_mutex_unlock((pthread_mutex_t *)mutex->mtx);
}
