#include <stdio.h>
#include <stdlib.h>
#include "../src/em_mempool.h"

int main()
{
	em_mpool_t mp;
	
	int *idata[10];
	em_create_mpool(&mp, sizeof(int), 10);
	em_print_mpool(&mp);

	printf("alloc\n");
	for (int i = 0; i < 5; i++)
	{
		em_alloc_block(&mp, (void **)&idata[i]);
		*idata[i] = i + 1;
	}
	em_print_mpool(&mp);

	// printf("free\n");
	em_free_block(&mp, idata[0]);
	em_print_mpool(&mp);
	em_free_block(&mp, idata[2]);
	em_print_mpool(&mp);

	printf("alloc2\n");
	for (int i = 5; i < 10; i++)
	{
		em_alloc_block(&mp, (void **)&idata[i]);
		*idata[i] = i + 1;
		em_print_mpool(&mp);
	}
	em_free_block(&mp, idata[5]);
	em_print_mpool(&mp);
	em_free_block(&mp, idata[9]);
	em_print_mpool(&mp);
	em_alloc_block(&mp, (void **)&idata[0]);
	*idata[0] = 11;
	em_print_mpool(&mp);

	
	em_delete_mpool(&mp);
}