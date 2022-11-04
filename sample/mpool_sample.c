#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/em_mempool.h"

//#define MALLOC
#define STATIC

#define BLOCK_SIZE sizeof(int)
#define BLOCK_NUM 10
static char memory[EM_MEMPOOL_CALC_MEMSIZE(BLOCK_SIZE, BLOCK_NUM)];

int main()
{
	em_mpool_t mp;
	int *idata[10];

#if defined MALLOC
	em_mpool_create(&mp, BLOCK_SIZE, BLOCK_NUM, &malloc, &free);

#elif defined STATIC
	int memsize_min = em_mpool_calc_memsize(BLOCK_SIZE, BLOCK_NUM);
	printf("mem: %ld / %d\n", sizeof(memory), memsize_min);
	em_mpool_create_with_mem2(&mp, BLOCK_SIZE, BLOCK_NUM, memory);
#endif

	em_mpool_print(&mp);

	printf("alloc\n");
	for (int i = 0; i < 5; i++)
	{
		em_mpool_alloc_block(&mp, (void **)&idata[i], 0);
		*idata[i] = i + 1;
	}
	em_mpool_print(&mp);

	// printf("free\n");
	em_mpool_free_block(&mp, idata[0]);
	em_mpool_print(&mp);
	em_mpool_free_block(&mp, idata[2]);
	em_mpool_print(&mp);

	printf("alloc2\n");
	for (int i = 5; i < 10; i++)
	{
		em_mpool_alloc_block(&mp, (void **)&idata[i], 0);
		*idata[i] = i + 1;
		em_mpool_print(&mp);
	}
	em_mpool_free_block(&mp, idata[5]);
	em_mpool_print(&mp);
	em_mpool_free_block(&mp, idata[9]);
	em_mpool_print(&mp);
	em_mpool_alloc_block(&mp, (void **)&idata[0], 0);
	*idata[0] = 11;
	em_mpool_print(&mp);

	uint data_num;
	printf("data_ptr array1: ");
	int *dataPtr[10];
	em_mpool_get_dataptr_array(&mp, 10, &data_num, (void **)dataPtr);
	for (int i = 0; i < data_num; i++)
	{
		printf("[%d]", *dataPtr[i]);
	}
	printf("\n");

	printf("data_ptr array2: ");
	for (int i = 0; i < mp.num_used; i++)
	{
		int *val = mp.data_ptr[i];
		printf("[%d]", *val);
	}
	printf("\n");

	printf("data_idx array: ");
	uint dataIdx[10];
	em_mpool_get_dataidx_array(&mp, 10, &data_num, dataIdx);
	for (int i = 0; i < data_num; i++)
	{
		printf("[%d]", dataIdx[i]);
	}
	printf("\n");

	em_mpool_destroy(&mp);
}