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
#include <memory.h>
#include <string.h>

#include "em_datamng.h"
#include "em_print.h"

static int _em_datamng_init(em_datamng_t *dm, em_idcnt_t *idcnt)
{
	dm->idcnt = idcnt;
	for (int i = 0; i < dm->mp.num_max; i++)
	{
		dm->idcnt[i].id = EM_DATAMNG_INVALID_ID;
		dm->idcnt[i].count = 0;
	}
	em_mutex_init(&dm->mutex);

	return 0;
}

int em_datamng_create_with_mem(em_datamng_t *dm,
							   int data_size,
							   int data_num,
							   int duplicate_mode,
							   em_blkinfo_t **block_ptr,
							   em_blkinfo_t *block,
							   void *rawdata,
							   em_idcnt_t *idcnt)
{
	dm->duplicate_mode = duplicate_mode;
	em_mpool_create_with_mem(&dm->mp, data_size, data_num,
							 block_ptr, block, rawdata);

	return _em_datamng_init(dm, idcnt);
}

int em_datamng_create(em_datamng_t *dm, int data_size, int data_num, int duplicate_mode,
					  void *(*allc_func)(size_t),
					  void (*free_func)(void *))
{
	dm->duplicate_mode = duplicate_mode;
	dm->free_func = free_func;
	em_idcnt_t *idcnt = (em_idcnt_t *)allc_func(sizeof(em_idcnt_t) * data_num);

	em_mpool_create(&dm->mp, data_size, data_num, allc_func, free_func);
	// dm->idcnt = (em_idcnt_t *)malloc(sizeof(em_idcnt_t) * data_size);

	return _em_datamng_init(dm, idcnt);
}

int em_datamng_delete(em_datamng_t *dm)
{
	em_mpool_delete(&dm->mp);
	dm->free_func(dm->idcnt);
	em_mutex_destroy(&dm->mutex);

	return 0;
}

int em_datamng_print(em_datamng_t *dm)
{
	printf("print %d %d %d ", dm->mp.num_max, dm->mp.num_used, dm->mp.block_size);

	for (int i = 0; i < dm->mp.num_max; i++)
	{
		if (i == dm->mp.num_used)
		{
			printf("   ");
		}
		printf("[%ld:%d:%d] ",
			   dm->idcnt[dm->mp.block_ptr[i]->index].id,
			   dm->idcnt[dm->mp.block_ptr[i]->index].count,
			   *(int *)(dm->mp.block_ptr[i]->data_ptr));
	}
	printf("\n");

	return 0;
}

// unsafe
static int _em_datamng_get_dataidx(em_datamng_t *dm, ulong id)
{
	int ret = -1;
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (dm->idcnt[data_index].id == id)
		{
			ret = data_index;
			break;
		}
	}
	return ret;
}

// unsafe
static int _em_datamng_get_blockinfo(em_datamng_t *dm, ulong id, em_blkinfo_t **block)
{
	int ret = -1;
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (dm->idcnt[data_index].id == id)
		{
			*block = dm->mp.block_ptr[i];
			ret = 0;
			break;
		}
	}
	return ret;
}

int em_datamng_add_data(em_datamng_t *dm, ulong id, void *data)
{
	int ret = -1;
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);

	int exist_dataidx = _em_datamng_get_dataidx(dm, id);
	if (exist_dataidx < 0) // no data
	{
		void *new_data_ptr;
		if (0 != em_mpool_alloc_block(&dm->mp, &new_data_ptr, EM_NO_TIMEOUT))
		{
			em_printf(EM_LOG_DEBUG, "add data error\n");
		}
		else
		{
			int dataidx = em_mpool_get_dataidx(&dm->mp, new_data_ptr);
			memcpy(new_data_ptr, data, dm->mp.block_size);
			dm->idcnt[dataidx].id = id;
			dm->idcnt[dataidx].count = 1;
			ret = 0;
		}
	}
	else // data exists
	{
		if (dm->duplicate_mode == EM_DMNG_DPLCT_ERROR)
		{
			em_printf(EM_LOG_DEBUG, "id %ld already exists\n", id);
		}
		else if (dm->duplicate_mode == EM_DMNG_DPLCT_UPDATE)
		{
			void *dst = em_mpool_get_dataptr(&dm->mp, exist_dataidx);
			memcpy(dst, data, dm->mp.block_size);
			ret = 0;
		}
		else if (dm->duplicate_mode == EM_DMNG_DPLCT_COUNTUP)
		{
			dm->idcnt[exist_dataidx].count++;
			ret = 0;
		}
		else
		{
			em_printf(EM_LOG_ERROR, "unknown duplicatemode: %d\n", dm->duplicate_mode);
		}
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

// unsafe
static void *_em_datamng_get_data_ptr(em_datamng_t *dm, ulong id)
{
	em_blkinfo_t *block_tmp;

	if (0 != _em_datamng_get_blockinfo(dm, id, &block_tmp))
	{
		return NULL;
	}

	return block_tmp->data_ptr;
}

void *em_datamng_get_data_ptr(em_datamng_t *dm, ulong id)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	void *ret = _em_datamng_get_data_ptr(dm, id);
	em_mutex_unlock(&dm->mutex);

	return ret;
}

int em_datamng_get_dataidx(em_datamng_t *dm, ulong id)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = _em_datamng_get_dataidx(dm, id);
	em_mutex_unlock(&dm->mutex);

	return ret;
}

void *em_datamng_get_dataptr_by_dataidx(em_datamng_t *dm, uint data_idx)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	void *ret = em_mpool_get_dataptr(&dm->mp, data_idx);
	em_mutex_unlock(&dm->mutex);

	return ret;
}

int em_datamng_get_data(em_datamng_t *dm, ulong id, void *data)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = -1;

	void *data_ptr = _em_datamng_get_data_ptr(dm, id);
	if (data_ptr != NULL)
	{
		memcpy(data, data_ptr, dm->mp.block_size);
		ret = 0;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

int em_datamng_get_data_cnt(em_datamng_t *dm, ulong id)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = -1;
	em_blkinfo_t *block_tmp;
	if (0 == _em_datamng_get_blockinfo(dm, id, &block_tmp))
	{
		ret = dm->idcnt[block_tmp->index].count;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

int em_datamng_remove_data(em_datamng_t *dm, ulong id)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = -1;
	em_blkinfo_t *block_tmp;
	if (0 == _em_datamng_get_blockinfo(dm, id, &block_tmp))
	{
		dm->idcnt[block_tmp->index].count--;
		if (dm->idcnt[block_tmp->index].count <= 0)
		{
			em_mpool_free_block_by_dataidx(&dm->mp, block_tmp->index);
		}
		ret = 0;
	}
	em_mutex_unlock(&dm->mutex);
	return ret;
}

ulong em_datamng_get_id(em_datamng_t *dm,
								void *searchdata)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	ulong ret = EM_DATAMNG_INVALID_ID;

	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		// if (comparator(dm->mp.block_ptr[i]->data_ptr, searchdata))
		if (0 == memcmp(dm->mp.block_ptr[i]->data_ptr, searchdata, dm->mp.block_size))
		{
			ret = dm->idcnt[i].id;
			break;
		}
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

// unsafe
static int _em_datamng_get_data_index_by_func(em_datamng_t *dm,
											  void *searchdata,
											  char (*comparator)(void *, void *))
{
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (comparator(dm->mp.block[data_index].data_ptr, searchdata))
		{
			return data_index;
		}
	}

	return -1;
}

ulong em_datamng_get_id_by_func(em_datamng_t *dm,
										void *searchdata,
										char (*comparator)(void *, void *))
{
	ulong ret = EM_DATAMNG_INVALID_ID;
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);

	int data_index = _em_datamng_get_data_index_by_func(dm, searchdata, comparator);
	if (data_index >= 0)
	{
		ret = dm->idcnt[data_index].id;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

int em_datamng_get_data_by_func(em_datamng_t *dm,
								void *searchdata,
								char (*comparator)(void *, void *),
								void *data)
{
	int ret = -1;
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);

	int data_index = _em_datamng_get_data_index_by_func(dm, searchdata, comparator);
	if (data_index >= 0)
	{
		memcpy(data, dm->mp.block[data_index].data_ptr, dm->mp.block_size);
		ret = 0;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}
