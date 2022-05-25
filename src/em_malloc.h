#ifndef __EM_MALLOC_H__
#define __EM_MALLOC_H__

#include "em_mempool.h"

typedef struct _em_meminfo_t em_meminfo_t;

struct _em_meminfo_t
{
	int mem_index;
	int mem_length;
	em_meminfo_t *next_meminfo;
	em_meminfo_t *back_meminfo;
	char is_used;
};

typedef struct
{
	int mem_total_size;
	int mem_unit_size; //割り当て単位
	em_mpool_t mp_used;
	em_mpool_t mp_free;
	void *memory;
	em_meminfo_t first_meminfo;
	em_meminfo_t last_meminfo;
} em_memmng_t;

int em_set_meminfo_t(em_meminfo_t *minfo,
					 int mem_index, int mem_length, char is_used,
					 em_meminfo_t *next_meminfo,
					 em_meminfo_t *back_meminfo);
// static int _em_init_datamng(em_memmng_t *mm, em_blockadd_t *adddata);

// int em_create_datamng_with_mem(em_memmng_t *mm,
//								int mem_size,
//								int data_num,
//								em_blockmng_t **block_ptr,
//								em_blockmng_t *block,
//								void *rawdata,
//								em_blockadd_t *adddata);

int em_create_memmng(em_memmng_t *mm,
					 int mem_total_size,
					 int mem_unit_size);

int em_delete_memmng(em_memmng_t *mm);

int em_print_memmng(em_memmng_t *mm);

void *em_malloc(em_memmng_t *mm, int size);

int em_free(em_memmng_t *mm, void *addr);

#endif //__EM_MALLOC_H__
