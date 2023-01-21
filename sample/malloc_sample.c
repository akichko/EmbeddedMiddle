#include <stdio.h>
#include "../src/em_malloc.h"
#include "../src/em_print.h"

//#define MALLOC
#define STATIC

#define BLOCK_SIZE 16
#define BLOCK_NUM 50
#define ALLOC_MAX_NUM 100

static char memory[EM_MEMMNG_CALC_MEMSIZE(BLOCK_SIZE, BLOCK_NUM, ALLOC_MAX_NUM)];

static char memory2[10000];

int main()
{
	em_memmng_t mm;
	void *pt[10];
	int detail = 1;

	em_print_set_loglevel(EM_LOG_TRACE);
	printf("block size = %d\n", BLOCK_SIZE);
	
#if defined MALLOC
	em_memmng_create(&mm, BLOCK_SIZE, BLOCK_NUM, ALLOC_MAX_NUM, NULL);
	
#elif defined STATIC
	printf("mem size: %ld\n", sizeof(memory));
	em_memmng_create(&mm, BLOCK_SIZE, BLOCK_NUM, ALLOC_MAX_NUM, memory);
#endif

	em_memmng_print(&mm, detail);

	//printf("0: malloc 16\n");
	pt[0] = em_malloc(&mm, 16);
	em_memmng_print(&mm, detail);

	//printf("1: malloc 25\n");
	pt[1] = em_malloc(&mm, 25);
	//printf("2: malloc 35\n");
	pt[2] = em_malloc(&mm, 35);
	//printf("3: malloc 32\n");
	pt[3] = em_malloc(&mm, 32);
	//printf("4: malloc 32\n");
	pt[4] = em_malloc(&mm, 32);
	em_memmng_print(&mm, detail);

	//printf("2: free\n");
	em_free(&mm, pt[2]);
	em_memmng_print(&mm, detail);

	//printf("3: free\n");
	em_free(&mm, pt[3]);
	em_memmng_print(&mm, detail);

	//printf("1: free\n");
	em_free(&mm, pt[1]);
	em_memmng_print(&mm, detail);

	//printf("5: malloc 6\n");
	pt[5] = em_malloc(&mm, 64);
	em_memmng_print(&mm, detail);

	//printf("6: trymalloc 64 100ms\n");
	pt[6] = em_trymalloc(&mm, 64, 100);
	em_memmng_print(&mm, detail);
	// em_print_mpool(&mm.mp_used);

	//printf("7: malloc 1600\n");
	pt[7] = em_malloc(&mm, 1600);
	if (pt[7] == NULL)
	{
		printf("allocation failed\n");
	}
	printf("7: trymalloc 1600 NO_WAIT\n");
	pt[7] = em_trymalloc(&mm, 1600, EM_NO_WAIT);
	if (pt[7] == NULL)
	{
		printf("allocation failed\n");
	}
	printf("7: trymalloc 1600 1000ms\n");
	pt[7] = em_trymalloc(&mm, 1600, 1000);
	if (pt[7] == NULL)
	{
		printf("allocation failed timeout 1000ms\n");
	}
	printf("7: trymalloc 1600 1000ms\n");
	pt[7] = em_trymalloc(&mm, 1600, 1000);
	if (pt[7] == NULL)
	{
		printf("allocation failed timeout 1000ms\n");
	}
	em_memmng_print(&mm, detail);
	// em_print_mpool(&mm.mp_used);

	em_free(&mm, pt[0]);
	em_memmng_print(&mm, detail);

	em_free(&mm, pt[6]);
	em_memmng_print(&mm, detail);

	em_free(&mm, pt[5]);
	em_memmng_print(&mm, detail);

	em_free(&mm, pt[4]);
	em_memmng_print(&mm, detail);

	em_memmng_destroy(&mm);
}