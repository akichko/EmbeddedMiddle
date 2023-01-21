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

#include "em_gdatamng.h"
#include "em_print.h"

static int _em_gdatamng_keycmp(char key_type, const void *s1, const void *s2, size_t n)
{
	if (key_type == EM_DMNG_KEY_STRING)
		return strncmp((const char *)s1, (const char *)s2, n);
	else
		return memcmp(s1, s2, n);
}

int em_gdatamng_create(em_datamng_t *dm, uint data_size, uint data_num,
					   char key_type, uint key_size, char duplicate_mode,
					   void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	dm->key_size = key_size;
	dm->key_type = key_type;
	dm->duplicate_mode = duplicate_mode;
	dm->free_func = free_func;
	dm->keycnt = (em_keycnt_t *)alloc_func(sizeof(em_keycnt_t) * data_num);
	dm->keymem = alloc_func(key_size * data_num);

	em_mpool_create(&dm->mp, data_size, data_num, alloc_func, free_func);

	for (uint i = 0; i < data_num; i++)
	{
		dm->keycnt[i].key = (char *)dm->keymem + i * dm->key_size;
		dm->keycnt[i].count = 0;
	}
	em_mutex_init(&dm->mutex);

	return 0;
}

int em_gdatamng_destroy(em_datamng_t *dm)
{
	em_mpool_destroy(&dm->mp);
	dm->free_func(dm->keycnt);
	dm->free_func(dm->keymem);
	em_mutex_destroy(&dm->mutex);

	return 0;
}

int em_gdatamng_print(em_datamng_t *dm)
{
	printf("print %d %d %d %d ", dm->mp.num_max, dm->mp.num_used, dm->key_size, dm->mp.block_size);
	long ikey;
	for (uint i = 0; i < dm->mp.num_max; i++)
	{
		if (i == dm->mp.num_used)
		{
			printf("   ");
		}
		printf("[");
		if (dm->key_type == EM_DMNG_KEY_STRING)
		{
			printf("%.8s", (char *)dm->keycnt[dm->mp.block_ptr[i]->index].key);
		}
		else if (dm->key_type == EM_DMNG_KEY_INTEGER)
		{
			ikey = 0;
			memcpy(&ikey, dm->keycnt[dm->mp.block_ptr[i]->index].key, dm->key_size);
			printf("%ld", ikey);
		}
		else
		{
			em_printf(EM_LOG_ERROR, "unknown key type\n");
		}
		printf(":%d:%d] ",
			   dm->keycnt[dm->mp.block_ptr[i]->index].count,
			   *(int *)(dm->mp.block_ptr[i]->data_ptr));
	}
	printf("\n");

	return 0;
}

// unsafe
static int _em_gdatamng_get_dataidx(em_datamng_t *dm, const void *key)
{
	int ret = -1;
	int data_index;
	for (uint i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (0 == _em_gdatamng_keycmp(dm->key_type, dm->keycnt[data_index].key, key, dm->key_size))
		{
			ret = data_index;
			break;
		}
	}
	return ret;
}

// unsafe
static int _em_gdatamng_get_blockinfo(em_datamng_t *dm, const void *key, em_blkinfo_t **block)
{
	int ret = -1;
	int data_index;
	for (uint i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (0 == _em_gdatamng_keycmp(dm->key_type, dm->keycnt[data_index].key, key, dm->key_size))
		{
			*block = dm->mp.block_ptr[i];
			ret = 0;
			break;
		}
	}
	return ret;
}

int em_gdatamng_add_data(em_datamng_t *dm, const void *key, const void *data)
{
	int ret = -1;
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);

	int exist_dataidx = _em_gdatamng_get_dataidx(dm, key);
	if (exist_dataidx < 0) // no data
	{
		void *new_data_ptr;
		if (0 != em_mpool_alloc_block(&dm->mp, &new_data_ptr, EM_NO_WAIT))
		{
			em_printf(EM_LOG_DEBUG, "add data error\n");
		}
		else
		{
			int dataidx = em_mpool_get_dataidx(&dm->mp, new_data_ptr);
			memcpy(new_data_ptr, data, dm->mp.block_size);
			memcpy(dm->keycnt[dataidx].key, key, dm->key_size);
			dm->keycnt[dataidx].count = 1;
			ret = 0;
		}
	}
	else // data exists
	{
		if (dm->duplicate_mode == EM_DMNG_DPLCT_ERROR)
		{
			em_printf(EM_LOG_ERROR, "key already exists\n");
		}
		else if (dm->duplicate_mode == EM_DMNG_DPLCT_UPDATE)
		{
			void *dst = em_mpool_get_dataptr(&dm->mp, exist_dataidx);
			memcpy(dst, data, dm->mp.block_size);
			ret = 0;
		}
		else if (dm->duplicate_mode == EM_DMNG_DPLCT_COUNTUP)
		{
			dm->keycnt[exist_dataidx].count++;
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
static void *_em_gdatamng_get_data_ptr(em_datamng_t *dm, const void *key)
{
	em_blkinfo_t *block_tmp;

	if (0 != _em_gdatamng_get_blockinfo(dm, key, &block_tmp))
	{
		return NULL;
	}

	return block_tmp->data_ptr;
}

void *em_gdatamng_get_data_ptr(em_datamng_t *dm, const void *key)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	void *ret = _em_gdatamng_get_data_ptr(dm, key);
	em_mutex_unlock(&dm->mutex);

	return ret;
}

int em_gdatamng_get_dataidx(em_datamng_t *dm, const void *key)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = _em_gdatamng_get_dataidx(dm, key);
	em_mutex_unlock(&dm->mutex);

	return ret;
}

void *em_gdatamng_get_dataptr_by_dataidx(em_datamng_t *dm, uint data_idx)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	void *ret = em_mpool_get_dataptr(&dm->mp, data_idx);
	em_mutex_unlock(&dm->mutex);

	return ret;
}

int em_gdatamng_get_data(em_datamng_t *dm, const void *key, void *data)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = -1;

	void *data_ptr = _em_gdatamng_get_data_ptr(dm, key);
	if (data_ptr != NULL)
	{
		memcpy(data, data_ptr, dm->mp.block_size);
		ret = 0;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

int em_gdatamng_get_data_cnt(em_datamng_t *dm, const void *key)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = -1;
	em_blkinfo_t *block_tmp;
	if (0 == _em_gdatamng_get_blockinfo(dm, key, &block_tmp))
	{
		ret = dm->keycnt[block_tmp->index].count;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

int em_gdatamng_remove_data(em_datamng_t *dm, const void *key)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	int ret = -1;
	em_blkinfo_t *block_tmp;
	if (0 == _em_gdatamng_get_blockinfo(dm, key, &block_tmp))
	{
		dm->keycnt[block_tmp->index].count--;
		if (dm->keycnt[block_tmp->index].count <= 0)
		{
			em_mpool_free_block_by_dataidx(&dm->mp, block_tmp->index);
		}
		ret = 0;
	}
	em_mutex_unlock(&dm->mutex);
	return ret;
}

void *em_gdatamng_get_key(em_datamng_t *dm,
						  void *searchdata)
{
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);
	void *ret = NULL;

	for (uint i = 0; i < dm->mp.num_used; i++)
	{
		// if (comparator(dm->mp.block_ptr[i]->data_ptr, searchdata))
		if (0 == memcmp(dm->mp.block_ptr[i]->data_ptr, searchdata, dm->mp.block_size))
		{
			ret = dm->keycnt[i].key;
			break;
		}
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

// unsafe
static int _em_gdatamng_get_data_index_by_func(em_datamng_t *dm,
											   void *searchdata,
											   char (*comparator)(void *, void *))
{
	int data_index;
	for (uint i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (comparator(dm->mp.block[data_index].data_ptr, searchdata))
		{
			return data_index;
		}
	}

	return -1;
}

void *em_gdatamng_get_key_by_func(em_datamng_t *dm,
								  void *searchdata,
								  char (*comparator)(void *, void *))
{
	void *ret = NULL;
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);

	int data_index = _em_gdatamng_get_data_index_by_func(dm, searchdata, comparator);
	if (data_index >= 0)
	{
		ret = dm->keycnt[data_index].key;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

int em_gdatamng_get_data_by_func(em_datamng_t *dm,
								 void *searchdata,
								 char (*comparator)(void *, void *),
								 void *data)
{
	int ret = -1;
	em_mutex_lock(&dm->mutex, EM_NO_TIMEOUT);

	int data_index = _em_gdatamng_get_data_index_by_func(dm, searchdata, comparator);
	if (data_index >= 0)
	{
		memcpy(data, dm->mp.block[data_index].data_ptr, dm->mp.block_size);
		ret = 0;
	}

	em_mutex_unlock(&dm->mutex);
	return ret;
}

int em_gdatamng_get_data_num(em_datamng_t *dm)
{
	return dm->mp.num_used;
}


int em_gdatamng_get_keyval_by_idx(em_datamng_t *dm, uint elem_idx, em_keyval_t *dst)
{
	if (elem_idx >= dm->mp.num_used || dst == NULL)
	{
		return -1;
	}

	int data_index = dm->mp.block_ptr[elem_idx]->index;

	dst->key = dm->keycnt[data_index].key;
	dst->val = dm->mp.block_ptr[elem_idx]->data_ptr;

	return 0;
}
