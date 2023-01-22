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
#include <stdlib.h>
#include <memory.h>
#include "em_queue.h"
#include "em_mutex.h"
#include "em_print.h"

int em_queue_create_with_mem(em_queue_t *qu,
							 int block_size,
							 int block_num,
							 void **block_ptr,
							 void *rawdata)
{
	qu->num_max = block_num;
	qu->num_used = 0;
	qu->block_size = block_size;
	qu->block_ptr = block_ptr;
	qu->rawdata = rawdata;
	qu->head_ptr = 0;
	qu->tail_ptr = 0;
	em_sem_init(&qu->sem_out, 0);
	em_sem_init(&qu->sem_in, block_num);
	em_mutex_init(&qu->mutex);

	for (int i = 0; i < qu->num_max; i++)
	{
		qu->block_ptr[i] = (char *)qu->rawdata + block_size * i;
	}

	return 0;
}

int em_queue_create(em_queue_t *qu,
					int block_size,
					int block_num,
					void *(*alloc_func)(size_t),
					void (*free_func)(void *))
{
	qu->free_func = free_func;
	void *rawdata = alloc_func(block_size * block_num);
	void **block_ptr = (void **)alloc_func(sizeof(void *) * block_num);

	return em_queue_create_with_mem(qu, block_size, block_num, block_ptr, rawdata);
}

int em_queue_destroy(em_queue_t *qu)
{
	qu->free_func(qu->block_ptr);
	qu->free_func(qu->rawdata);
	em_sem_destroy(&qu->sem_out);
	em_sem_destroy(&qu->sem_in);
	em_mutex_destroy(&qu->mutex);

	return 0;
}

int em_queue_print(em_queue_t *qu)
{
	em_printf(EM_LOG_TOP, "print: usage=%d/%d bsize=%d head=%d tail=%d\n",
			  qu->num_used, qu->num_max,
			  qu->block_size, qu->head_ptr, qu->tail_ptr);

	return 0;
}

// unsafe
static void *_em_enqueue_get_dataptr(em_queue_t *qu)
{
	if (qu->num_used >= qu->num_max)
		return NULL;

	return qu->block_ptr[qu->head_ptr];
}

// unsafe
static int _em_enqueue_increment(em_queue_t *qu)
{
	if (qu->num_used >= qu->num_max)
		return -1;

	qu->num_used++;

	qu->head_ptr++;
	if (qu->head_ptr >= qu->num_max)
	{
		qu->head_ptr -= qu->num_max;
	}
	return 0;
}

int em_enqueue(em_queue_t *qu, void *block_data, int timeout_ms)
{
	int ret;
	void *tmp;
	// lock
	if (0 != em_sem_wait(&qu->sem_in, timeout_ms))
	{
		return -1;
	}
	if (0 != em_mutex_lock(&qu->mutex, EM_WAIT))
	{
		em_printf(EM_LOG_ERROR, "lock error\n");
		em_sem_post(&qu->sem_in);
		return -1;
	}

	tmp = _em_enqueue_get_dataptr(qu);
	if (tmp == NULL)
	{
		em_printf(EM_LOG_ERROR, "Fatal Error: get_dataptr failed\n");
		em_mutex_unlock(&qu->mutex);
		em_sem_post(&qu->sem_in);
		return -1;
	}

	memcpy(tmp, block_data, qu->block_size);

	ret = _em_enqueue_increment(qu);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "Fatal Error: increment failed\n");
	}
	// unlock
	em_mutex_unlock(&qu->mutex);
	em_sem_post(&qu->sem_out);

	return 0;
}

// unsafe
static void *_em_dequeue_get_dataptr(em_queue_t *qu)
{
	if (qu->num_used <= 0)
		return NULL;

	return qu->block_ptr[qu->tail_ptr];
}

// unsafe
static int _em_dequeue_increment(em_queue_t *qu)
{
	if (qu->num_used <= 0)
		return -1;

	qu->num_used--;

	qu->tail_ptr++;
	if (qu->tail_ptr >= qu->num_max)
	{
		qu->tail_ptr -= qu->num_max;
	}

	return 0;
}

int em_dequeue(em_queue_t *qu, void *block_data, int timeout_ms)
{
	int ret;
	void *data_ptr;

	ret = em_sem_wait(&qu->sem_out, timeout_ms);
	if (ret != 0)
	{
		// printf("em_dequeue timeout\n");
		return EM_ERR_TIMEOUT;
	}
	// lock
	ret = em_mutex_lock(&qu->mutex, EM_WAIT);
	if (ret != 0)
	{
		em_printf(EM_LOG_ERROR, "lock error\n");
		em_sem_post(&qu->sem_out);
		return EM_ERR_OTHER;
	}

	data_ptr = _em_dequeue_get_dataptr(qu);

	if (data_ptr == NULL)
	{
		//キューが空なら最初のセマフォではじかれるので想定外
		em_printf(EM_LOG_ERROR, "Fatal queue error\n");
		em_sem_post(&qu->sem_out);
		em_mutex_unlock(&qu->mutex);
		return EM_ERR_OTHER;
	}

	memcpy(block_data, data_ptr, qu->block_size);

	ret = _em_dequeue_increment(qu);
	// unlock
	em_mutex_unlock(&qu->mutex);
	em_sem_post(&qu->sem_in);
	return 0;
}

int em_queue_getnum(em_queue_t *qu)
{
	int ret;

	// lock
	if (0 != em_mutex_lock(&qu->mutex, EM_NO_WAIT))
	{
		return -1;
	}

	ret = qu->num_used;

	// unlock
	em_mutex_unlock(&qu->mutex);
	return ret;
}
