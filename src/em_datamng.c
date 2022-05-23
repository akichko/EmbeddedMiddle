#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "em_datamng.h"

static int _em_init_datamng(em_datamng_t *dm, em_blockadd_t *adddata)
{
	dm->adddata = adddata;
	for (int i = 0; i < dm->mp.num_max; i++)
	{
		dm->adddata[i].id = -1;
		dm->adddata[i].count = 0;
	}

	return 0;
}

int em_create_datamng_with_mem(em_datamng_t *dm,
								int data_size,
								int data_num,
								em_blockmng_t **block_ptr,
								em_blockmng_t *block,
								void *rawdata,
								em_blockadd_t *adddata)
{
	em_create_mpool_with_mem(&dm->mp, data_num, data_size,
							  block_ptr, block, rawdata);

	return _em_init_datamng(dm, adddata);
}

int em_create_datamng(em_datamng_t *dm, int data_size, int data_num)
{
	em_blockadd_t *adddata = (em_blockadd_t *)malloc(sizeof(em_blockadd_t) * data_size);

	em_create_mpool(&dm->mp, data_num, data_size);
	//dm->adddata = (em_blockadd_t *)malloc(sizeof(em_blockadd_t) * data_size);

	return _em_init_datamng(dm, adddata);
}

int em_delete_datamng(em_datamng_t *dm)
{
	em_delete_mpool(&dm->mp);
	free(dm->adddata);

	return 0;
}

int em_print_datamng(em_datamng_t *dm)
{
	printf("print %d %d %d ", dm->mp.num_max, dm->mp.num_used, dm->mp.block_size);

	for (int i = 0; i < dm->mp.num_max; i++)
	{
		if (i == dm->mp.num_used)
		{
			printf("   ");
		}
		printf("[%d:%d:%d] ",
			   dm->adddata[dm->mp.block_ptr[i]->index].id,
			   dm->adddata[dm->mp.block_ptr[i]->index].count,
			   *(int *)(dm->mp.block_ptr[i]->data_ptr));
	}
	printf("\n");

	return 0;
}

int em_get_block(em_datamng_t *dm, int id, em_blockmng_t **block)
{
	// em_blockmng_t *tmp_block;
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (dm->adddata[data_index].id == id)
		{
			*block = dm->mp.block_ptr[i];
			return 0;
		}
	}
	return -1;
}

int em_set_data(em_datamng_t *dm, void *data, int id)
{
	em_blockmng_t *block_tmp;
	int ret = em_get_block(dm, id, &block_tmp);
	if (ret != 0)
	{
		// em_blockadd_t *adddata;
		em_alloc_blockmng(&dm->mp, &block_tmp);
		memcpy(block_tmp->data_ptr, data, dm->mp.block_size);
		dm->adddata[block_tmp->index].id = id;
	}
	dm->adddata[block_tmp->index].count++;

	return 0;
}

int em_del_block(em_datamng_t *dm, int id)
{
	int data_index;
	for (int i = 0; i < dm->mp.num_used; i++)
	{
		data_index = dm->mp.block_ptr[i]->index;
		if (dm->adddata[data_index].id == id)
		{
			dm->adddata[data_index].count--;
			if (dm->adddata[data_index].count <= 0)
			{
				// dm->adddata[data_index].id = -1;
				em_free_block_by_dataidx(&dm->mp, data_index);
			}
			return 0;
		}
	}
	return -1;
}
