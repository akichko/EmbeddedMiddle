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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "em_time.h"
#include "em_semaphore.h"

int em_sem_init(em_sem_t *sem, int value)
{
	return sem_init(&sem->smf, 0, value);
}

int em_sem_destroy(em_sem_t *sem)
{
	sem_destroy(&sem->smf);
	return 0;
}

int em_sem_wait(em_sem_t *sem, int timeout_ms)
{
	if (timeout_ms == EM_NO_TIMEOUT)
	{
		return 0 == sem_wait(&sem->smf) ? EM_SUCCESS : EM_ERROR;
	}
	else
	{
		struct timespec ts = em_get_offset_timestamp(timeout_ms);
		return 0 == sem_timedwait(&sem->smf, &ts) ? EM_SUCCESS : EM_E_TIMEOUT;
	}
}

int em_sem_post(em_sem_t *sem)
{
	return sem_post(&sem->smf);
}

