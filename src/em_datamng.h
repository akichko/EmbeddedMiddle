#ifndef __EM_DATAMNG_H__
#define __EM_DATAMNG_H__

#include "em_mempool.h"

typedef struct
{
	unsigned long id;
	int count;
} em_idcnt_t;

typedef struct
{
	em_mpool_t mp;
	em_idcnt_t *idcnt;
} em_datamng_t;

static int _em_datamng_init(em_datamng_t *dm,
							em_idcnt_t *idcnt);

int em_datamng_create_with_mem(em_datamng_t *dm,
							   int data_size,
							   int data_num,
							   em_blkinfo_t **block_ptr,
							   em_blkinfo_t *block,
							   void *rawdata,
							   em_idcnt_t *idcnt);

int em_datamng_create(em_datamng_t *dm,
					  int data_size,
					  int data_num);

int em_datamng_delete(em_datamng_t *dm);

int em_datamng_print(em_datamng_t *dm);

int em_datamng_get_block(em_datamng_t *dm,
				 unsigned long id,
				 em_blkinfo_t **block);

int em_datamng_set_data(em_datamng_t *dm,
				unsigned long id,
				void *data);

void *em_datamng_get_data_ptr(em_datamng_t *dm,
					  unsigned long id);

int em_datamng_get_data(em_datamng_t *dm,
				unsigned long id,
				void *data);

int em_datamng_del_block(em_datamng_t *dm,
				 unsigned long id);

#endif //__EM_DATAMNG_H__
