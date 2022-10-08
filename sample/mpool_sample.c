#include <stdio.h>
#include <stdlib.h>
#include "../src/em_mempool.h"

int main()
{
	em_mpool_t mp;

	int *idata[10];
	em_mpool_create(&mp, sizeof(int), 10, &malloc, &free);
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
	printf("*data_ptr array: ");
	int *dataPtr[10];
	em_mpool_get_dataptr_array(&mp, 10, &data_num, (void **)dataPtr);
	for (int i = 0; i < data_num; i++)
	{
		printf("[%d]", *dataPtr[i]);
	}
	printf("\n");

	printf("data_idx array: ");
	uint dataIdx[10];
	em_mpool_get_dataidx_array(&mp, 10, &data_num, (void*)&dataIdx);
	for (int i = 0; i < data_num; i++)
	{
		printf("[%d]", dataIdx[i]);
	}
	printf("\n");

	em_mpool_delete(&mp);
}