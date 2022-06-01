#ifndef __EM_MUTEX_H__
#define __EM_MUTEX_H__

#include <pthread.h>
#include <semaphore.h>

#define EM_NO_TIMEOUT -1
#define EM_NO_WAIT 0

//mutex
typedef struct
{
	pthread_mutex_t mtx;
} em_mutex_t;

int em_mutex_init(em_mutex_t *mutex);

int em_mutex_destroy(em_mutex_t *mutex);

int em_mutex_lock(em_mutex_t *mutex, int timeout_ms);

int em_mutex_unlock(em_mutex_t *mutex);


//semaphore
typedef struct
{
	sem_t smf;
} em_sem_t;

int em_sem_init(em_sem_t *sem, int value);

int em_sem_destroy(em_sem_t *sem);

int em_sem_wait(em_sem_t *sem, int timeout_ms);

int em_sem_post(em_sem_t *sem);

#endif //__EM_MUTEX_H__
