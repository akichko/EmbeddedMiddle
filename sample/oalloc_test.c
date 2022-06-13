#include <stdio.h>
#include "../src/em_oalloc.h"

int main()
{
	em_omemmng_t mm;
	void *pt[10];

	printf("block size = 16\n");
	em_omemmng_create(&mm, 800, 12, NULL);
	em_omemmng_print(&mm);
	
	pt[0] = em_oalloc(&mm, 16);
	em_omemmng_print(&mm);

	pt[1] = em_oalloc(&mm, 25);
	em_omemmng_print(&mm);

	pt[2] = em_oalloc(&mm, 35);
	em_omemmng_print(&mm);

	pt[3] = em_oalloc(&mm, 32);
	em_omemmng_print(&mm);

	pt[4] = em_oalloc(&mm, 64);
	em_omemmng_print(&mm);

	pt[5] = em_oalloc(&mm, 700);
	em_omemmng_print(&mm);

	pt[6] = em_oalloc(&mm, 70);
	em_omemmng_print(&mm);
	
	em_omemmng_delete(&mm);
}