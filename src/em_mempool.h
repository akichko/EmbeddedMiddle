#ifndef __EM_MEMPOOL_H__
#define __EM_MEMPOOL_H__

#include "em_mutex.h"

typedef struct
{
	int index;		// block index
	int index_ptr;	// search index from
	void *data_ptr; // raw data
} em_blkinfo_t;

typedef struct
{
	int block_size;
	int num_max;
	int num_used;
	em_blkinfo_t **block_ptr; // dearch index
	em_blkinfo_t *block;
	void *rawdata; //実体
	em_sem_t sem;
	em_mutex_t mutex;
} em_mpool_t;

int em_mpool_create_with_mem(em_mpool_t *mp,
							 int block_size,
							 int block_num,
							 em_blkinfo_t **block_ptr,
							 em_blkinfo_t *block,
							 void *rawdata);

int em_mpool_create(em_mpool_t *mp,
					int block_size,
					int block_num);

int em_mpool_delete(em_mpool_t *mp);

int em_mpool_print(em_mpool_t *mp);

int em_mpool_alloc_block(em_mpool_t *mp,
						 void **block_data,
						 int timeout_ms);

int _em_mpool_alloc_blockmng(em_mpool_t *mp,
							 em_blkinfo_t **block_mng);

int em_mpool_get_dataidx(em_mpool_t *mp,
						 void *block_data);

int em_mpool_free_block_by_dataidx(em_mpool_t *mp,
								   int del_offset);

int em_mpool_free_block(em_mpool_t *mp,
						void *block_data);

#endif //__EM_MEMPOOL_H__
