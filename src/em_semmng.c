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
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include "em_time.h"
#include "em_semmng.h"
#include "em_mempool.h"
#include "em_cmndefs.h"

int em_semmng_init(em_semmng_t *smm, int max_sem_num,
					void *(*allc_func)(size_t),
					void(*free_func)(void *))
{
	smm->free_func = free_func;
	em_mpool_create(&smm->mp_semaphore, sizeof(sem_t), max_sem_num, allc_func, free_func);
}

em_semp_t em_semmng_factory(em_semmng_t *smm, int value)
{
	sem_t *sem;
	if (0 != em_mpool_alloc_block(&smm->mp_semaphore, (void **)&sem, EM_NO_WAIT))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return NULL;
	}
	sem_init(sem, 0, value);
	return sem;
}

int em_semmng_dispose(em_semmng_t *smm, em_semp_t sem_p)
{
	if (0 != sem_destroy((sem_t *)sem_p))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	return em_mpool_free_block(&smm->mp_semaphore, sem_p);
}

int em_semmng_wait(em_semp_t sem_p, int timeout_ms)
{
	if (timeout_ms == EM_WAIT)
	{
		return sem_wait((sem_t *)sem_p);
	}
	else
	{
		struct timespec ts = em_get_offset_timestamp(timeout_ms);
		return sem_timedwait((sem_t *)sem_p, &ts);
	}
}

int em_semmng_post(em_semp_t sem_p)
{
	return sem_post((sem_t *)sem_p);
}

int em_semmng_semp2id(em_semmng_t *smm, em_semp_t sem_p)
{
	return em_mpool_get_dataidx(&smm->mp_semaphore, sem_p);
}

em_semp_t em_semmng_semid2p(em_semmng_t *smm, int sem_id)
{
	return em_mpool_get_dataptr(&smm->mp_semaphore, sem_id);
	//return (em_semp_t)smm->mp_semaphore.block[sem_id].data_ptr;
}
