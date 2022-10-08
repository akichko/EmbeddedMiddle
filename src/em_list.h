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
#ifndef __EM_LIST_H__
#define __EM_LIST_H__

#include "em_mutex.h"
#include "em_mempool.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _em_listitem_t em_listitem_t;

struct _em_listitem_t
{
	void *data;
	em_listitem_t *next;
	em_listitem_t *back;
};

typedef struct
{
	em_mpool_t mp;
	em_listitem_t *first;
	em_listitem_t *last;
	uint count;
} em_list_t;

int em_list_create(em_list_t *li,
				   int num_max_item,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_list_delete(em_list_t *li);

int em_list_add(em_list_t *li,
				void *data);

int em_list_remove_at(em_list_t *li,
					  uint index);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_LIST_H__
