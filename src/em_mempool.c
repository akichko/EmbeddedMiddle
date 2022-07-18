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
#include "em_print.h"

int em_mpool_create_with_mem(em_mpool_t *mp,
							 int block_size,
							 int block_num,
							 em_blkinfo_t **block_ptr,
							 em_blkinfo_t *block,
							 void *rawdata)
{
	mp->num_max = block_num;
	mp->num_used = 0;
	mp->block_size = block_size;
	mp->block_ptr = block_ptr;
	mp->block = block;
	mp->rawdata = rawdata;
	em_sem_init(&mp->sem, block_num);
	em_mutex_init(&mp->mutex);

	for (int i = 0; i < mp->num_max; i++)
	{
		mp->block_ptr[i] = &mp->block[i];
		mp->block[i].index = i;
		mp->block[i].index_ptr = i;
		mp->block[i].data_ptr = mp->rawdata + block_size * i;
	}

	return 0;
}

int em_mpool_create(em_mpool_t *mp, int block_size, int block_num,
					void *(*allc_func)(size_t),
					void (*free_func)(void *))
{
	mp->free_func = free_func;
	em_blkinfo_t **block_ptr = (em_blkinfo_t **)allc_func(sizeof(em_blkinfo_t *) * block_num);
	em_blkinfo_t *block = (em_blkinfo_t *)allc_func(sizeof(em_blkinfo_t) * block_num);
	void *rawdata = allc_func(block_size * block_num);

	return em_mpool_create_with_mem(mp, block_size, block_num,
									block_ptr, block, rawdata);
}

int em_mpool_delete(em_mpool_t *mp)
{
	mp->free_func(mp->block_ptr);
	mp->free_func(mp->block);
	mp->free_func(mp->rawdata);
	em_sem_destroy(&mp->sem);
	em_mutex_destroy(&mp->mutex);
	return 0;
}

int em_mpool_print(em_mpool_t *mp)
{
	em_printf(EM_LOG_TOP, "print %d %d %d ", mp->num_max, mp->num_used, mp->block_size);

	for (int i = 0; i < mp->num_max; i++)
	{
		if (i == mp->num_used)
		{
			em_printf(EM_LOG_TOP, "   ");
		}
		em_printf(EM_LOG_TOP, "[%ld:%d:%d] ",
			   (mp->block_ptr[i]->data_ptr - mp->rawdata) / mp->block_size,
			   mp->block[i].index_ptr,
			   *(int *)(mp->block_ptr[i]->data_ptr));
	}
	em_printf(EM_LOG_TOP, "\n");

	return 0;
}

// unsafe
int _em_mpool_alloc_blockmng(em_mpool_t *mp, em_blkinfo_t **block_mng)
{
	if (mp->num_used >= mp->num_max)
	{
		return -1;
	}

	*block_mng = mp->block_ptr[mp->num_used];
	mp->num_used++;

	return 0;
}

int em_mpool_alloc_block(em_mpool_t *mp, void **block_data, int timeout_ms)
{
	// lock
	if (0 != em_sem_wait(&mp->sem, timeout_ms))
		return -1;

	em_mutex_lock(&mp->mutex, EM_NO_TIMEOUT);

	em_blkinfo_t *block_mng;
	int ret = -1;
	ret = _em_mpool_alloc_blockmng(mp, &block_mng);
	if (ret == 0)
	{
		*block_data = block_mng->data_ptr;
		ret = 0;
	}

	em_mutex_unlock(&mp->mutex);
	return ret;
}

int em_mpool_get_dataidx(em_mpool_t *mp, void *block_data)
{
	int data_offset = (block_data - mp->rawdata) / mp->block_size;

	return data_offset;
}

void *em_mpool_get_dataptr(em_mpool_t *mp, uint data_idx)
{
	if(mp->block[data_idx].index_ptr >= mp->num_used)
		return NULL;

	return mp->block[data_idx].data_ptr;
}

int em_mpool_free_block_by_dataidx(em_mpool_t *mp, int del_offset)
{
	em_mutex_lock(&mp->mutex, EM_NO_TIMEOUT);

	int del_index = mp->block[del_offset].index_ptr;
	if (mp->num_used <= 0 || del_index >= mp->num_used)
	{
		// unlock
		em_printf(EM_LOG_ERROR, "em_free_block index error\n");
		return -1;
	}

	mp->num_used--;
	int swap_offset = mp->block_ptr[mp->num_used]->index;

	mp->block[swap_offset].index_ptr = del_index;
	mp->block[del_offset].index_ptr = mp->num_used;
	mp->block_ptr[del_index] = &mp->block[swap_offset];
	mp->block_ptr[mp->num_used] = &mp->block[del_offset];

	em_mutex_unlock(&mp->mutex);
	em_sem_post(&mp->sem);
	return 0;
}

int em_mpool_free_block(em_mpool_t *mp, void *block_data)
{
	int data_offset = (block_data - mp->rawdata) / mp->block_size;

	em_printf(EM_LOG_TRACE, "free idx=%d val=%d\n", mp->block[data_offset].index_ptr, *(int *)block_data);

	return em_mpool_free_block_by_dataidx(mp, data_offset); // with lock
}
