#include <stdio.h>
#include <stdlib.h>
#include "em_mempool.h"

int em_create_mpool_with_mem(em_mpool_t *mp,
							  int block_num,
							  int block_size,
							  em_blockmng_t **block_ptr,
							  em_blockmng_t *block,
							  void *rawdata)
{
	mp->num_max = block_num;
	mp->num_used = 0;
	mp->block_size = block_size;
	mp->block_ptr = block_ptr;
	mp->block = block;
	mp->rawdata = rawdata;

	for (int i = 0; i < mp->num_max; i++)
	{
		mp->block_ptr[i] = &mp->block[i];
		mp->block[i].index = i;
		mp->block[i].index_ptr = i;
		mp->block[i].data_ptr = mp->rawdata + block_size * i;
	}

	return 0;
}

int em_create_mpool(em_mpool_t *mp, int block_num, int block_size)
{
	em_blockmng_t **block_ptr = (em_blockmng_t **)malloc(sizeof(em_blockmng_t *) * block_num);
	em_blockmng_t *block = (em_blockmng_t *)malloc(sizeof(em_blockmng_t) * block_num);
	void *rawdata = malloc(block_size * block_num);

	return em_create_mpool_with_mem(mp, block_num, block_size,
									 block_ptr, block, rawdata);
}

int em_delete_mpool(em_mpool_t *mp)
{
	free(mp->block_ptr);
	free(mp->block);
	free(mp->rawdata);

	return 0;
}

int em_print_mpool(em_mpool_t *mp)
{
	printf("print %d %d %d ", mp->num_max, mp->num_used, mp->block_size);

	for (int i = 0; i < mp->num_max; i++)
	{
		if (i == mp->num_used)
		{
			printf("   ");
		}
		printf("[%ld:%d:%d] ",
			   (mp->block_ptr[i]->data_ptr - mp->rawdata) / mp->block_size,
			   mp->block[i].index_ptr,
			   *(int *)(mp->block_ptr[i]->data_ptr));
	}
	printf("\n");

	return 0;
}

int em_alloc_blockmng(em_mpool_t *mp, em_blockmng_t **block_mng)
{
	if (mp->num_used >= mp->num_max)
		return -1;

	*block_mng = mp->block_ptr[mp->num_used];
	mp->num_used++;

	return 0;
}

int em_alloc_block(em_mpool_t *mp, void **block_data)
{
	em_blockmng_t *block_mng;
	int ret = em_alloc_blockmng(mp, &block_mng);
	if (ret != 0)
	{
		return -1;
	}
	*block_data = block_mng->data_ptr;

	return 0;
}

int em_free_block_by_dataidx(em_mpool_t *mp, int data_offset)
{
	if (mp->num_used <= 0)
		return -1;

	int index = mp->block[data_offset].index_ptr;

	mp->num_used--;

	mp->block[mp->num_used].index_ptr = index;
	mp->block[data_offset].index_ptr = mp->num_used;
	mp->block_ptr[index] = &mp->block[mp->num_used];
	mp->block_ptr[mp->num_used] = &mp->block[data_offset];

	return 0;
}

int em_free_block(em_mpool_t *mp, void *block_data)
{
	int data_offset = (block_data - mp->rawdata) / mp->block_size;

	printf("free idx=%d val=%d\n", mp->block[data_offset].index_ptr, *(int *)block_data);

	return em_free_block_by_dataidx(mp, data_offset);
}
