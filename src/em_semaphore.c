#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include "em_mutex.h"
#include "em_time.h"
#include "em_timer.h"

int em_sem_init(em_sem_t *sem, int value)
{
	//sem->smf = malloc(sizeof(sem_t));
	return sem_init(&sem->smf, 0, value);
}

int em_sem_destroy(em_sem_t *sem)
{
	sem_destroy(&sem->smf);
	//free(sem->smf);
	return 0;
}

int em_sem_wait(em_sem_t *sem, int timeout_ms)
{
	if (timeout_ms == EM_NO_TIMEOUT)
	{
		return sem_wait(&sem->smf);
	}
	else
	{
		struct timespec ts = em_get_offset_timestamp(timeout_ms);
		//if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
		//	printf("Error: clock_gettime\n");
		//// ts= em_convert_time(timeout_ms);
		//ts.tv_sec += timeout_ms / 1000;
		return sem_timedwait(&sem->smf, &ts);
	}
}

int em_sem_post(em_sem_t *sem)
{
	return sem_post(&sem->smf);
}
