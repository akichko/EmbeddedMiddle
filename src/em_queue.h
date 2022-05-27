#ifndef __EM_QUEUE_H__
#define __EM_QUEUE_H__

typedef struct
{
	int block_size;
	int num_max;
	int num_used;
	int head_ptr;
	int tail_ptr;
	void **block_ptr;
	void *rawdata; //実体
} em_queue_t;

int em_create_queue_with_mem(em_queue_t *qu,
							 int block_size,
							 int block_num,
							 void **block_ptr,
							 void *rawdata);

int em_create_queue(em_queue_t *qu,
					int block_size,
					int block_num);

int em_delete_queue(em_queue_t *qu);

int em_print_queue(em_queue_t *qu);

void *em_get_enqueue_get_dataptr(em_queue_t *qu);

int em_enqueue_increment(em_queue_t *qu);

int em_enqueue(em_queue_t *qu, void *block_data);

void *em_dequeue_get_dataptr(em_queue_t *qu);

int em_dequeue_increment(em_queue_t *qu);

int em_dequeue(em_queue_t *qu, void *block_data);

#endif //__EM_QUEUE_H__
