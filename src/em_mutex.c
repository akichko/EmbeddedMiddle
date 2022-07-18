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
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "em_mutex.h"
#include "em_time.h"
#include "em_timer.h"
#include "em_cmndefs.h"

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
			em_printf(EM_LOG_ERROR, "Fatal Error\n");
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
