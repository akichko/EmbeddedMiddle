#ifndef __EM_DATAMNG_H__
#define __EM_DATAMNG_H__

#include "em_mempool.h"

#define EM_DMNG_DPLCT_ERROR 1
#define EM_DMNG_DPLCT_UPDATE 2
#define EM_DMNG_DPLCT_COUNTUP 3

#define EM_DATAMNG_INVALID_ID 0xffffffffffffffff

typedef struct
{
	ulong id;
	int count;
} em_idcnt_t;

typedef struct
{
	em_mpool_t mp;
	em_idcnt_t *idcnt;
	int duplicate_mode;
	em_mutex_t mutex;
	void (*free_func)(void *);
} em_datamng_t;

int em_datamng_create_with_mem(em_datamng_t *dm,
							   int data_size,
							   int data_num,
							   int duplicate_mode,
							   em_blkinfo_t **block_ptr,
							   em_blkinfo_t *block,
							   void *rawdata,
							   em_idcnt_t *idcnt);

int em_datamng_create(em_datamng_t *dm,
					  int data_size,
					  int data_num,
					  int duplicate_mode,
					  void *(*allc_func)(size_t),
					  void (*free_func)(void *));

int em_datamng_delete(em_datamng_t *dm);

int em_datamng_print(em_datamng_t *dm);

int em_datamng_add_data(em_datamng_t *dm,
						ulong id,
						void *data);

void *em_datamng_get_data_ptr(em_datamng_t *dm,
							  ulong id);

int em_datamng_get_dataidx(em_datamng_t *dm,
						   ulong id);

void *em_datamng_get_dataptr_by_dataidx(em_datamng_t *dm,
										uint data_idx);

int em_datamng_get_data(em_datamng_t *dm,
						ulong id,
						void *data);

int em_datamng_get_data_cnt(em_datamng_t *dm,
							ulong id);

int em_datamng_remove_data(em_datamng_t *dm,
						   ulong id);

ulong em_datamng_get_id(em_datamng_t *dm,
						void *searchdata);

ulong em_datamng_get_id_by_func(em_datamng_t *dm,
								void *searchdata,
								char (*comparator)(void *, void *));

int em_datamng_get_data_by_func(em_datamng_t *dm,
								void *searchdata,
								char (*comparator)(void *, void *),
								void *data);

#endif //__EM_DATAMNG_H__
