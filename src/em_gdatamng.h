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
#ifndef __EM_GDATAMNG_H__
#define __EM_GDATAMNG_H__

#include "em_mempool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EM_DMNG_KEY_INTEGER 1
#define EM_DMNG_KEY_STRING 2

#define EM_DMNG_DPLCT_ERROR 1
#define EM_DMNG_DPLCT_UPDATE 2
#define EM_DMNG_DPLCT_COUNTUP 3

typedef struct
{
	void *key;
	int count;
} em_keycnt_t;

typedef struct
{
	em_mpool_t mp;
	em_keycnt_t *keycnt;
	uint key_size;
	void *keymem;
	char duplicate_mode;
	char key_type;
	em_mutex_t mutex;
	void (*free_func)(void *);
} em_datamng_t;

int em_gdatamng_create(em_datamng_t *dm,
					   uint data_size,
					   uint data_num,
					   char key_type,
					   uint key_size,
					   char duplicate_mode,
					   void *(*alloc_func)(size_t),
					   void (*free_func)(void *));

int em_gdatamng_delete(em_datamng_t *dm);

int em_gdatamng_print(em_datamng_t *dm);

int em_gdatamng_add_data(em_datamng_t *dm,
						 const void *key,
						 const void *data);

void *em_gdatamng_get_data_ptr(em_datamng_t *dm,
							   const void *key);

int em_gdatamng_get_dataidx(em_datamng_t *dm,
							const void *key);

void *em_gdatamng_get_dataptr_by_dataidx(em_datamng_t *dm,
										 uint data_idx);

int em_gdatamng_get_data(em_datamng_t *dm,
						 const void *key,
						 void *data);

int em_gdatamng_get_data_cnt(em_datamng_t *dm,
							 const void *key);

int em_gdatamng_remove_data(em_datamng_t *dm,
							const void *key);

void *em_gdatamng_get_key(em_datamng_t *dm,
						  void *searchdata);

void *em_gdatamng_get_key_by_func(em_datamng_t *dm,
								  void *searchdata,
								  char (*comparator)(void *, void *));

int em_gdatamng_get_data_by_func(em_datamng_t *dm,
								 void *searchdata,
								 char (*comparator)(void *, void *),
								 void *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_GDATAMNG_H__
