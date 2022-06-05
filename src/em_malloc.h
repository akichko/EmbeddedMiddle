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
	int mem_unit_size;
	int mem_unit_bshift;
	int mem_used_bsize;
	em_mpool_t mp_used;
	em_mpool_t mp_free;
	void *memory;
	em_meminfo_t **minfo_ptr;
	em_meminfo_t first_meminfo;
	em_meminfo_t last_meminfo;
	em_mutex_t mutex;
} em_memmng_t;

int em_set_meminfo_t(em_meminfo_t *minfo,
					 int mem_index, int mem_length, char is_used,
					 em_meminfo_t *next_meminfo,
					 em_meminfo_t *back_meminfo);

int em_memmng_create(em_memmng_t *mm,
					 int mem_total_size,
					 int mem_unit_size);

int em_memmng_delete(em_memmng_t *mm);

int em_memmng_print(em_memmng_t *mm);

void *em_malloc(em_memmng_t *mm,
				size_t size);

void em_free(em_memmng_t *mm,
			void *addr);

#endif //__EM_MALLOC_H__
