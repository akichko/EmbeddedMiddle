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
#ifndef __EM_MEMPOOL_H__
#define __EM_MEMPOOL_H__

#include "em_mutex.h"
#include "em_semaphore.h"

typedef struct
{
	int index;		// block index
	int index_ptr;	// search index from
	void *data_ptr; // raw data
} em_blkinfo_t;

typedef struct
{
	uint block_size;
	uint num_max;
	uint num_used;
	em_blkinfo_t **block_ptr; // search index
	em_blkinfo_t *block;
	void *rawdata; //実体
	em_sem_t sem;
	em_mutex_t mutex;
	void (*free_func)(void *);
} em_mpool_t;

int em_mpool_create_with_mem(em_mpool_t *mp,
							 uint block_size,
							 uint block_num,
							 em_blkinfo_t **block_ptr,
							 em_blkinfo_t *block,
							 void *rawdata);

int em_mpool_create(em_mpool_t *mp,
					uint block_size,
					uint block_num,
					void *(*alloc_func)(size_t),
					void (*free_func)(void *));

int em_mpool_delete(em_mpool_t *mp);

int em_mpool_print(em_mpool_t *mp);

int em_mpool_alloc_block(em_mpool_t *mp,
						 void **block_data,
						 int timeout_ms);

int em_mpool_get_dataidx(em_mpool_t *mp,
						 void *block_data);

void *em_mpool_get_dataptr(em_mpool_t *mp,
						   uint data_idx);

int em_mpool_free_block_by_dataidx(em_mpool_t *mp,
								   uint data_idx);

int em_mpool_free_block(em_mpool_t *mp,
						void *block_data);

int em_mpool_get_dataptr_array(em_mpool_t *mp,
							   uint max_size,
							   uint *data_num,
							   void **data_ptrs);

int em_mpool_get_dataidx_array(em_mpool_t *mp,
							   uint max_size,
							   uint *data_num,
							   uint *data_idxs);

#endif //__EM_MEMPOOL_H__
