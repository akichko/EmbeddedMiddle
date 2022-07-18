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
#ifndef __EM_MUTEX_H__
#define __EM_MUTEX_H__

#include <pthread.h>
#include <semaphore.h>
#include "em_cmndefs.h"

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
