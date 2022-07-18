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
#ifndef __EM_QUEUE_H__
#define __EM_QUEUE_H__

#include "em_mutex.h"

typedef struct
{
	int block_size;
	int num_max;
	int num_used;
	int head_ptr;
	int tail_ptr;
	void **block_ptr;
	void *rawdata; //実体
	em_sem_t sem_in;
	em_sem_t sem_out;
	em_mutex_t mutex;
} em_queue_t;

int em_queue_create_with_mem(em_queue_t *qu,
							 int block_size,
							 int block_num,
							 void **block_ptr,
							 void *rawdata);

int em_queue_create(em_queue_t *qu,
					int block_size,
					int block_num);

int em_queue_delete(em_queue_t *qu);

int em_queue_print(em_queue_t *qu);

void *em_enqueue_get_dataptr(em_queue_t *qu);

int em_enqueue_increment(em_queue_t *qu);

int em_enqueue(em_queue_t *qu,
			   void *block_data,
			   int timeout_ms);

void *em_dequeue_get_dataptr(em_queue_t *qu);

int em_dequeue_increment(em_queue_t *qu);

int em_dequeue(em_queue_t *qu,
			   void *block_data,
			   int timeout_ms);

int em_queue_getnum(em_queue_t *qu,
					int timeout_ms);

#endif //__EM_QUEUE_H__
