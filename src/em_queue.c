#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "em_queue.h"

int em_create_queue_with_mem(em_queue_t *qu,
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

	for (int i = 0; i < qu->num_max; i++)
	{
		qu->block_ptr[i] = qu->rawdata + block_size * i;
	}

	return 0;
}

int em_create_queue(em_queue_t *qu,
					int block_size,
					int block_num)
{
	void *rawdata = malloc(block_size * block_num);
	void **block_ptr = (void **)malloc(sizeof(void *) * block_num);

	return em_create_queue_with_mem(qu, block_size, block_num, block_ptr, rawdata);
}

int em_delete_queue(em_queue_t *qu)
{
	free(qu->block_ptr);
	free(qu->rawdata);

	return 0;
}

int em_print_queue(em_queue_t *qu)
{
	printf("print: %d / %d, bsize=%d head=%d tail=%d\n",
		   qu->num_used, qu->num_max,
		   qu->block_size, qu->head_ptr, qu->tail_ptr);

	return 0;
}

void *em_get_enqueue_get_dataptr(em_queue_t *qu)
{
	if (qu->num_used >= qu->num_max)
		return NULL;

	if (qu->head_ptr + 1 < qu->num_max)
		return qu->block_ptr[qu->head_ptr + 1];
	else
		return qu->block_ptr[0];
}

int em_enqueue_increment(em_queue_t *qu)
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

int em_enqueue(em_queue_t *qu, void *block_data)
{
	void *tmp;
	tmp = em_get_enqueue_get_dataptr(qu);
	if (tmp == NULL)
		return -1;

	memcpy(tmp, block_data, qu->block_size);

	return em_enqueue_increment(qu);
}

void *em_dequeue_get_dataptr(em_queue_t *qu)
{
	if (qu->num_used <= 0)
		return NULL;

	if (qu->tail_ptr + 1 < qu->num_max)
		return qu->block_ptr[qu->tail_ptr + 1];
	else
		return qu->block_ptr[0];

}

int em_dequeue_increment(em_queue_t *qu)
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

int em_dequeue(em_queue_t *qu, void *block_data)
{
	void *tmp;
	tmp = em_dequeue_get_dataptr(qu);

	if (tmp == NULL)
		return -1;

	memcpy(block_data, tmp, qu->block_size);

	return em_dequeue_increment(qu);
}
