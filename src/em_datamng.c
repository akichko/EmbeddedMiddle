#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "em_datamng.h"

static int _em_datamng_init(em_datamng_t *dm, em_idcnt_t *idcnt)
{
	dm->idcnt = idcnt;
	for (int i = 0; i < dm->mp.num_max; i++)
	{
		dm->idcnt[i].id = -1;
		dm->idcnt[i].count = 0;
	}

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

int em_datamng_create(em_datamng_t *dm, int data_size, int data_num)
{
	em_idcnt_t *idcnt = (em_idcnt_t *)malloc(sizeof(em_idcnt_t) * data_num);

	em_mpool_create(&dm->mp, data_size, data_num);
	// dm->idcnt = (em_idcnt_t *)malloc(sizeof(em_idcnt_t) * data_size);

	return _em_datamng_init(dm, idcnt);
}

int em_datamng_delete(em_datamng_t *dm)
{
	em_mpool_delete(&dm->mp);
	free(dm->idcnt);

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

int em_datamng_get_block(em_datamng_t *dm, unsigned long id, em_blkinfo_t **block)
{
	// em_blkinfo_t *tmp_block;
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (dm->idcnt[data_index].id == id)
		{
			*block = dm->mp.block_ptr[i];
			return 0;
		}
	}
	return -1;
}

int em_datamng_set_data(em_datamng_t *dm, unsigned long id, void *data)
{
	em_blkinfo_t *block_tmp;
	int ret = em_datamng_get_block(dm, id, &block_tmp);
	if (ret != 0)
	{
		// em_idcnt_t *idcnt;
		em_mpool_alloc_blockmng(&dm->mp, &block_tmp);
		memcpy(block_tmp->data_ptr, data, dm->mp.block_size);
		dm->idcnt[block_tmp->index].id = id;
		dm->idcnt[block_tmp->index].count = 0;
	}

	dm->idcnt[block_tmp->index].count++;

	return 0;
}

void *em_datamng_get_data_ptr(em_datamng_t *dm, unsigned long id)
{
	em_blkinfo_t *block_tmp;

	int ret = em_datamng_get_block(dm, id, &block_tmp);
	if (ret != 0)
	{
		return NULL;
	}

	return block_tmp->data_ptr;
}

int em_datamng_get_data(em_datamng_t *dm, unsigned long id, void *data)
{
	// em_blkinfo_t *block_tmp;
	// int ret = em_datamng_get_block(dm, id, &block_tmp);
	void *data_ptr = em_datamng_get_data_ptr(dm, id);
	if (data_ptr == NULL)
	{
		return -1;
	}

	memcpy(data, data_ptr, dm->mp.block_size);

	return 0;
}

int em_datamng_del_block(em_datamng_t *dm, unsigned long id)
{
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (dm->idcnt[data_index].id == id)
		{
			dm->idcnt[data_index].count--;
			if (dm->idcnt[data_index].count <= 0)
			{
				// dm->idcnt[data_index].id = -1;
				em_mpool_free_block_by_dataidx(&dm->mp, data_index);
			}
			return 0;
		}
	}
	return -1;
}
