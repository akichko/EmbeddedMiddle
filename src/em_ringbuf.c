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
#include <stdio.h>
#include "em_ringbuf.h"

int em_ring_create(em_ring_t *rb,
				   uint block_size,
				   uint block_num,
				   uint override_mode,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *))
{
	rb->num_max = block_num;
	rb->num_used = 0;
	rb->block_size = block_size;
	rb->override_mode = override_mode;
	rb->free_func = free_func;
	rb->rawdata = alloc_func(block_size * block_num);
	rb->block_ptr = (void **)alloc_func(sizeof(void *) * block_num);
	rb->head_ptr = 0;
	rb->tail_ptr = 0;

	for (uint i = 0; i < rb->num_max; i++)
	{
		rb->block_ptr[i] = (char *)rb->rawdata + block_size * i;
	}

	return 0;
}

int em_ring_destroy(em_ring_t *rb)
{
	rb->free_func(rb->block_ptr);
	rb->block_ptr = NULL;
	rb->free_func(rb->rawdata);
	rb->rawdata = NULL;
	return 0;
}

int em_ring_print(em_ring_t *rb)
{
	em_printf(EM_LOG_TOP, "print: usage=%d/%d bsize=%d head=%d tail=%d\n",
			  rb->num_used, rb->num_max,
			  rb->block_size, rb->head_ptr, rb->tail_ptr);

	return 0;
}

//最新バッファ
void *em_ring_get_dataptr_new(em_ring_t *rb)
{
	// buffer full
	if (rb->num_used >= rb->num_max)
	{
		if (rb->override_mode == EM_RINGBUF_ERROR)
		{
			return NULL;
		}
		else if (rb->override_mode == EM_RINGBUF_UPDATE)
		{
			em_ring_delete_taildata(rb, 1);
		}
		else
		{
			em_printf(EM_LOG_ERROR, "invalid override mode\n");
		}
	}

	return rb->block_ptr[rb->head_ptr];
}

//最新バッファ登録
int em_ring_add_newdata(em_ring_t *rb)
{
	if (rb->num_used >= rb->num_max)
		return -1;

	rb->num_used++;
	rb->head_ptr++;

	if (rb->head_ptr >= rb->num_max)
	{
		rb->head_ptr = rb->head_ptr % rb->num_max;
	}

	return 0;
}

//最新データ取得 offset=0:最新
void *em_ring_get_dataptr_head(em_ring_t *rb, uint offset)
{
	if (offset + 1 > rb->num_used)
		return NULL;

	int idx = rb->head_ptr - 1 - offset;
	if (idx < 0)
	{
		idx %= rb->num_max;
	}

	return rb->block_ptr[idx];
}

//最古データ取得 offset=0:最古
void *em_ring_get_dataptr_tail(em_ring_t *rb, uint offset)
{
	if (offset + 1 > rb->num_used)
		return NULL;

	uint idx = rb->tail_ptr + offset;
	if (idx >= rb->num_max)
	{
		idx %= rb->num_max;
	}

	return rb->block_ptr[idx];
}

//最古データ削除
int em_ring_delete_taildata(em_ring_t *rb, uint del_num)
{
	if (del_num == 0 || del_num > rb->num_used)
		return -1;

	rb->num_used -= del_num;
	rb->tail_ptr += del_num;
	if (rb->tail_ptr >= rb->num_max)
	{
		rb->tail_ptr %= rb->num_max;
	}

	return 0;
}
