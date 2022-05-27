#include <stdio.h>
#include "../src/em_malloc.h"
//#include "../src/em_mempool.h"

int main()
{
	em_memmng_t mm;
	void *pt[10];
	
	em_create_memmng(&mm, 800, 16);
	em_print_memmng(&mm);
	
	pt[0] = em_malloc(&mm, 16);
	em_print_memmng(&mm);

	pt[1] = em_malloc(&mm, 25);
	pt[2] = em_malloc(&mm, 35);
	pt[3] = em_malloc(&mm, 32);
	pt[4] = em_malloc(&mm, 32);
	em_print_memmng(&mm);

	em_free(&mm, pt[2]);
	em_print_memmng(&mm);

	em_free(&mm, pt[3]);
	em_print_memmng(&mm);

	em_free(&mm, pt[1]);
	em_print_memmng(&mm);

	pt[5] = em_malloc(&mm, 64);
	em_print_memmng(&mm);

	pt[6] = em_malloc(&mm, 64);
	em_print_memmng(&mm);
	//em_print_mpool(&mm.mp_used);

	pt[7] = em_malloc(&mm, 1600);
	em_print_memmng(&mm);
	//em_print_mpool(&mm.mp_used);

	em_free(&mm, pt[0]);
	em_print_memmng(&mm);

	em_free(&mm, pt[6]);
	em_print_memmng(&mm);

	em_free(&mm, pt[5]);
	em_print_memmng(&mm);

	em_free(&mm, pt[4]);
	em_print_memmng(&mm);
}