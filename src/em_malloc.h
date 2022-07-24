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
#ifndef __EM_MALLOC_H__
#define __EM_MALLOC_H__

#include <pthread.h>
#include "em_mempool.h"

typedef struct
{
	unsigned short is_used : 1;
	unsigned short is_index : 15;
} em_minfo_p_ex_t;

typedef struct
{
	int mem_index;
	short mem_length;
	char is_used;
	em_minfo_p_ex_t next_meminfo;
	em_minfo_p_ex_t back_meminfo;
} em_minfo_ex_t;

typedef struct _em_meminfo_t em_meminfo_t;

struct _em_meminfo_t
{
	int mem_index;
	short mem_length;
	char is_used;
	em_meminfo_t *next_meminfo;
	em_meminfo_t *back_meminfo;
};

typedef struct
{
	int mem_total_size;
	int mem_unit_size;
	int mem_unit_bshift;
	int mem_total_bnum;
	int mem_used_bsize;
	em_mpool_t mp_used;
	em_mpool_t mp_free;
	void *memory;
	em_meminfo_t **minfo_ptr;
	em_meminfo_t first_meminfo;
	em_meminfo_t last_meminfo;
	em_mutex_t mutex;
	pthread_cond_t cond;
	em_mutex_t cond_mutex;
} em_memmng_t;

int em_set_meminfo_t(em_meminfo_t *minfo,
					 int mem_index, int mem_length, char is_used,
					 em_meminfo_t *next_meminfo,
					 em_meminfo_t *back_meminfo);

int em_memmng_create(em_memmng_t *mm,
					 int mem_total_size,
					 int mem_unit_size,
					 int alloc_max_num);

int em_memmng_delete(em_memmng_t *mm);

int em_memmng_print(em_memmng_t *mm,
					int detail);

void *em_malloc(em_memmng_t *mm,
				size_t size);

void *em_trymalloc(em_memmng_t *mm,
				   size_t size,
				   int timeout_ms);

void em_free(em_memmng_t *mm,
			 void *addr);

#endif //__EM_MALLOC_H__
