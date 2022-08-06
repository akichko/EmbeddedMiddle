#include <stdio.h>
#include <stdlib.h>
#include "../src/em_oalloc.h"

int main()
{
	em_omemmng_t mm;
	void *pt[10];
	int blockSize = 16;

	printf("block size = %d\n", blockSize);
	em_omemmng_create(&mm, 800, blockSize, NULL, &malloc, &free);
	em_omemmng_print(&mm);
	
	pt[0] = em_oalloc(&mm, 16);
	printf("pt[0]: %p, ", pt[0]);
	em_omemmng_print(&mm);

	pt[1] = em_oalloc(&mm, 25);
	printf("pt[1]: %p, ", pt[1]);
	em_omemmng_print(&mm);

	pt[2] = em_oalloc(&mm, 35);
	printf("pt[2]: %p, ", pt[2]);
	em_omemmng_print(&mm);

	pt[3] = em_oalloc(&mm, 32);
	printf("pt[3]: %p, ", pt[3]);
	em_omemmng_print(&mm);

	pt[4] = em_oalloc(&mm, 64);
	printf("pt[4]: %p, ", pt[4]);
	em_omemmng_print(&mm);

	pt[5] = em_oalloc(&mm, 700);
	printf("pt[5]: %p, ", pt[5]);
	em_omemmng_print(&mm);

	pt[6] = em_oalloc(&mm, 70);
	printf("pt[6]: %p, ", pt[6]);
	em_omemmng_print(&mm);
	
	em_omemmng_delete(&mm);
}