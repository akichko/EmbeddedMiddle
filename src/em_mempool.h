#ifndef __EM_MEMPOOL_H__
#define __EM_MEMPOOL_H__

typedef struct
{
	int index;
	int index_ptr; // block->index
	void *data_ptr;
} em_blockmng_t;

typedef struct
{
	int block_size;
	int num_max;
	int num_used;
	em_blockmng_t **block_ptr;
	em_blockmng_t *block;
	void *rawdata; //実体
} em_mpool_t;

int em_create_mpool_with_mem(em_mpool_t *mp,
							  int block_size,
							  int block_num,
							  em_blockmng_t **block_ptr,
							  em_blockmng_t *block,
							  void *rawdata);

int em_create_mpool(em_mpool_t *mp,
					 int block_size,
					 int block_num);

int em_delete_mpool(em_mpool_t *mp);

int em_print_mpool(em_mpool_t *mp);

int em_alloc_block(em_mpool_t *mp,
					void **block_data);

int em_alloc_blockmng(em_mpool_t *mp,
					   em_blockmng_t **block_mng);

int em_free_block_by_dataidx(em_mpool_t *mp,
							  int del_offset);

int em_free_block(em_mpool_t *mp,
				   void *block_data);

#endif //__EM_MEMPOOL_H__
