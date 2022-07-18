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
#include <stdlib.h>
#include "em_mempool.h"
#include "em_mutex.h"
#include "em_mtxmng.h"
#include "em_cmndefs.h"

int em_mtxmng_init(em_mtxmng_t *mtxm, int max_mutex_num,
				   void *(*allc_func)(size_t),
				   void (*free_func)(void *))
{
	mtxm->free_func = free_func;
	mtxm->islock = (char *)malloc(sizeof(char) * max_mutex_num);
	em_mpool_create(&mtxm->mp_mutex, sizeof(em_mutex_t), max_mutex_num, allc_func, free_func);
}

int em_mtxmng_destroy(em_mtxmng_t *mtxm)
{
	mtxm->free_func(mtxm->islock);
	em_mpool_delete(&mtxm->mp_mutex);
}

int em_mtxmng_create_mutex(em_mtxmng_t *mtxm)
{
	em_mutex_t *mutex;
	if (0 != em_mpool_alloc_block(&mtxm->mp_mutex, (void **)&mutex, EM_NO_WAIT))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	em_mutex_init(mutex);
	return em_mpool_get_dataidx(&mtxm->mp_mutex, mutex);
}

int em_mtxmng_delete_mutex(em_mtxmng_t *mtxm, int mutex_id)
{
	em_mutex_t *mutex = em_mpool_get_dataptr(&mtxm->mp_mutex, mutex_id);
	em_mutex_destroy(mutex);
	return em_mpool_free_block_by_dataidx(&mtxm->mp_mutex, mutex_id);
}

int em_mtxmng_lock(em_mtxmng_t *mtxm, int mutex_id, int timeout_ms)
{
	em_mutex_t *mutex = em_mpool_get_dataptr(&mtxm->mp_mutex, mutex_id);

	if (0 != em_mutex_lock(mutex, timeout_ms))
	{
		return -1;
	}
	mtxm->islock[mutex_id] = 1;

	return 0;
}

int em_mtxmng_unlock(em_mtxmng_t *mtxm, int mutex_id)
{
	em_mutex_t *mutex = (em_mutex_t *)mtxm->mp_mutex.block[mutex_id].data_ptr;

	mtxm->islock[mutex_id] = 0;
	return em_mutex_unlock(mutex);
}
