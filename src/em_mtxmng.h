#ifndef __EM_MTXMNG_H__
#define __EM_MTXMNG_H__

#include "em_mempool.h"
#include "em_cmndefs.h"

typedef struct
{
	char *islock;
	em_mpool_t mp_mutex;
	void (*free_func)(void *);
} em_mtxmng_t;

int em_mtxmng_init(em_mtxmng_t *mtxm, int max_mutex_num,
					  void *(*allc_func)(size_t),
					  void (*free_func)(void *));

int em_mtxmng_destroy(em_mtxmng_t *mtxm);

int em_mtxmng_create_mutex(em_mtxmng_t *mtxm);

int em_mtxmng_delete_mutex(em_mtxmng_t *mtxm, int mutex_id);

int em_mtxmng_lock(em_mtxmng_t *mtxm, int mutex_id, int timeout_ms);

int em_mtxmng_unlock(em_mtxmng_t *mtxm, int mutex_id);

#endif //__EM_MTXMNG_H__
