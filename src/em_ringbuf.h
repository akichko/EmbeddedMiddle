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
#ifndef __EM_RINGBUF_H__
#define __EM_RINGBUF_H__

#include "em_cmndefs.h"

#define EM_RINGBUF_ERROR 1
#define EM_RINGBUF_UPDATE 2

typedef struct
{
	uint block_size;
	uint override_mode;
	uint num_max;
	uint num_used;
	uint head_ptr;
	uint tail_ptr;
	void **block_ptr;
	void *rawdata; //実体
	void (*free_func)(void *);
} em_ring_t;

int em_ring_create(em_ring_t *rb,
				   uint block_size,
				   uint block_num,
				   uint override_mode,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_ring_destroy(em_ring_t *rb);

int em_ring_print(em_ring_t *rb);

//最新バッファ
void *em_ring_get_dataptr_new(em_ring_t *rb);

//最新バッファ登録
int em_ring_add_newdata(em_ring_t *rb);

//最新データ取得 offset=0:最新
void *em_ring_get_dataptr_head(em_ring_t *rb,
							   uint offset);

//最古データ取得 offset=0:最古
void *em_ring_get_dataptr_tail(em_ring_t *rb,
							   uint offset);

//最古データ削除
int em_ring_delete_taildata(em_ring_t *rb,
							uint del_num);

// int em_ring_get_datanum(em_ring_t *rb);
#endif //__EM_RINGBUF_H__
