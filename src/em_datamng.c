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
							   em_blkinfo_t **block_ptr,
							   em_blkinfo_t *block,
							   void *rawdata,
							   em_idcnt_t *idcnt)
{
	em_mpool_create_with_mem(&dm->mp, data_size, data_num,
							 block_ptr, block, rawdata);

	return _em_datamng_init(dm, idcnt);
}

int em_datamng_create(em_datamng_t *dm, int data_size, int data_num,
					  void *(*allc_func)(size_t),
					  void (*free_func)(void *))
{
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
	em_printf(EM_LOG_ERROR, "print %d %d %d ", dm->mp.num_max, dm->mp.num_used, dm->mp.block_size);

	for (int i = 0; i < dm->mp.num_max; i++)
	{
		if (i == dm->mp.num_used)
		{
			em_printf(EM_LOG_ERROR, "   ");
		}
		em_printf(EM_LOG_ERROR, "[%ld:%d:%d] ",
			   dm->idcnt[dm->mp.block_ptr[i]->index].id,
			   dm->idcnt[dm->mp.block_ptr[i]->index].count,
			   *(int *)(dm->mp.block_ptr[i]->data_ptr));
	}
	em_printf(EM_LOG_ERROR, "\n");

	return 0;
}

// unsafe
int _em_datamng_get_blockinfo(em_datamng_t *dm, unsigned long id, em_blkinfo_t **block)
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

int em_datamng_add_data(em_datamng_t *dm, unsigned long id, void *data)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	em_blkinfo_t *block_tmp;
	int ret = _em_datamng_get_blockinfo(dm, id, &block_tmp);
	if (ret != 0)
	{
		if (0 != _em_mpool_alloc_blockmng(&dm->mp, &block_tmp))
		{
			ret = -1;
			// em_printf(EM_LOG_ERROR, "add data error\n");
		}
		else
		{
			memcpy(block_tmp->data_ptr, data, dm->mp.block_size);
			dm->idcnt[block_tmp->index].id = id;
			dm->idcnt[block_tmp->index].count = 0;
			dm->idcnt[block_tmp->index].count++;
			ret = 0;
		}
	}
	else
	{
		dm->idcnt[block_tmp->index].count++;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

// unsafe
void *_em_datamng_get_data_ptr(em_datamng_t *dm, unsigned long id)
{
	em_blkinfo_t *block_tmp;

	if (0 != _em_datamng_get_blockinfo(dm, id, &block_tmp))
	{
		return NULL;
	}

	return block_tmp->data_ptr;
}

void *em_datamng_get_data_ptr(em_datamng_t *dm, unsigned long id)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	void *ret = _em_datamng_get_data_ptr(dm, id);
	em_mutex_unlock(&dm->mutex);

	return ret;
}

int em_datamng_get_data(em_datamng_t *dm, unsigned long id, void *data)
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

int em_datamng_get_data_cnt(em_datamng_t *dm, unsigned long id)
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

int em_datamng_remove_data(em_datamng_t *dm, unsigned long id)
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

unsigned long em_datamng_get_id(em_datamng_t *dm,
								void *searchdata)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	unsigned long ret = EM_DATAMNG_INVALID_ID;

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
int _em_datamng_get_data_index_by_func(em_datamng_t *dm,
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

unsigned long em_datamng_get_id_by_func(em_datamng_t *dm,
										void *searchdata,
										char (*comparator)(void *, void *))
{
	unsigned long ret = EM_DATAMNG_INVALID_ID;
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);

#if 1
	int data_index = _em_datamng_get_data_index_by_func(dm, searchdata, comparator);
	if (data_index >= 0)
	{
		ret = dm->idcnt[data_index].id;
	}
#else
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (comparator(dm->mp.block[data_index].data_ptr, searchdata))
		{
			ret = dm->idcnt[data_index].id;
			break;
		}
	}
#endif

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
