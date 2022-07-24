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
#ifndef __EM_MTXMNG_H__
#define __EM_MTXMNG_H__

#include "em_mempool.h"
#include "em_cmndefs.h"

typedef struct
{
	char *islock;
	em_mpool_t mp_mutex;
	void (*free_func)(void *);
} em_mtxmng_t;

int em_mtxmng_init(em_mtxmng_t *mtxm, int max_mutex_num,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_mtxmng_destroy(em_mtxmng_t *mtxm);

int em_mtxmng_create_mutex(em_mtxmng_t *mtxm);

int em_mtxmng_delete_mutex(em_mtxmng_t *mtxm,
						   int mutex_id);

int em_mtxmng_lock(em_mtxmng_t *mtxm,
				   int mutex_id,
				   int timeout_ms);

int em_mtxmng_unlock(em_mtxmng_t *mtxm,
					 int mutex_id);

#endif //__EM_MTXMNG_H__
