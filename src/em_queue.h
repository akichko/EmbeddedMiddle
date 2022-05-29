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
	em_sem_t sem;
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

int em_enqueue(em_queue_t *qu, void *block_data, int timeout_ms);

void *em_dequeue_get_dataptr(em_queue_t *qu);

int em_dequeue_increment(em_queue_t *qu);

int em_dequeue(em_queue_t *qu, void *block_data, int timeout_ms);

#endif //__EM_QUEUE_H__
