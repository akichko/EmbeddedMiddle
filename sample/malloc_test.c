#include <stdio.h>
#include "../src/em_malloc.h"
#include "../src/em_mempool.h"

typedef struct
{
	int attr1;
	int attr2;
	int attr3;
} em_test_t;

int main()
{
	em_memmng_t mm;

	void *pt[10];
	
	em_create_memmng(&mm, 800, 16);
	em_print_memmng(&mm);
	
	pt[0] = em_malloc(&mm, 10);
	em_print_memmng(&mm);

	pt[1] = em_malloc(&mm, 25);
	em_print_memmng(&mm);

	pt[2] = em_malloc(&mm, 35);
	em_print_memmng(&mm);

	em_free(&mm, pt[1]);
	em_print_memmng(&mm);

	pt[3] = em_malloc(&mm, 50);
	em_print_memmng(&mm);
	//em_print_mpool(&mm.mp_used);

	em_free(&mm, pt[2]);
	em_print_memmng(&mm);
	//em_print_mpool(&mm.mp_used);

	pt[4] = em_malloc(&mm, 32);
	em_print_memmng(&mm);
	//em_print_mpool(&mm.mp_used);
}