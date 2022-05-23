#ifndef __EM_DATAMNG_H__
#define __EM_DATAMNG_H__

#include "em_mempool.h"

typedef struct
{
	int id;
	int count;
} em_blockadd_t;

typedef struct
{
	em_mpool_t mp;
	em_blockadd_t *adddata;
} em_datamng_t;

static int _em_init_datamng(em_datamng_t *dm, em_blockadd_t *adddata);

int em_create_datamng_with_mem(em_datamng_t *dm,
								int data_size,
								int data_num,
								em_blockmng_t **block_ptr,
								em_blockmng_t *block,
								void *rawdata,
								em_blockadd_t *adddata);

int em_create_datamng(em_datamng_t *dm,
					   int data_size,
					   int data_num);

int em_delete_datamng(em_datamng_t *dm);

int em_print_datamng(em_datamng_t *dm);

int em_get_block(em_datamng_t *dm, int id, em_blockmng_t **block);

int em_set_data(em_datamng_t *dm, void *data, int id);

int em_del_block(em_datamng_t *dm, int id);

#endif //__EM_DATAMNG_H__
