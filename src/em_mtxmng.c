#include <stdio.h>
#include <stdlib.h>
#include "em_mempool.h"
#include "em_mutex.h"
#include "em_mtxmng.h"


int em_mtxmng_init(em_mtxmng_t *mtxm, int max_mutex_num)
{
	mtxm->islock = (char *)malloc(sizeof(char) * max_mutex_num);
	em_mpool_create(&mtxm->mp_mutex, sizeof(em_mutex_t), max_mutex_num);
}

int em_mtxmng_destroy(em_mtxmng_t *mtxm)
{
	free(mtxm->islock);
	em_mpool_delete(&mtxm->mp_mutex);
}

int em_mtxmng_create_mutex(em_mtxmng_t *mtxm)
{
	em_mutex_t *mutex;
	if (0 != em_mpool_alloc_block(&mtxm->mp_mutex, (void **)&mutex, EM_NO_WAIT))
	{
		printf("error\n");
		return -1;
	}
	em_mutex_init(mutex);
	return em_mpool_get_dataidx(&mtxm->mp_mutex, mutex);
}

int em_mtxmng_delete_mutex(em_mtxmng_t *mtxm, int mutex_id)
{
	em_mutex_t *mutex = mtxm->mp_mutex.block[mutex_id].data_ptr;
	em_mutex_destroy(mutex);
	return em_mpool_free_block_by_dataidx(&mtxm->mp_mutex, mutex_id);
}

int em_mtxmng_lock(em_mtxmng_t *mtxm, int mutex_id, int timeout_ms)
{
	em_mutex_t *mutex = (em_mutex_t *)mtxm->mp_mutex.block[mutex_id].data_ptr;

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
