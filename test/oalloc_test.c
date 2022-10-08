#include <stdio.h>
#include <stdlib.h>
#include "em_oalloc.h"
#include "em_print.h"

int main()
{
	em_printf(EM_LOG_TOP, "oalooc test start\n");

	em_omemmng_t mm;
	void *pt[10];
	int totalSize = 80;
	int blockSize = 16;

	int dummy;
	void *dummy_ptr = &dummy;

	for(int i=0 ; i<10; i++){
		pt[i] = dummy_ptr;
	}

	 // 5 block
	if ( 0 != em_omemmng_create(&mm, totalSize, blockSize, NULL, &malloc, &free)){
		em_printf(EM_LOG_ERROR, "error\n");
		exit(1);
	}

	int i = 0;
	pt[i] = em_oalloc(&mm, 16); // 1 block
	if(pt[i] == dummy_ptr || pt[i] == NULL){
		em_printf(EM_LOG_ERROR, "error\n");
		exit(1);
	}

	i = 1;
	pt[i] = em_oalloc(&mm, 10); // 1 block
	if(pt[i] == dummy_ptr || pt[i] == NULL){
		em_printf(EM_LOG_ERROR, "error\n");
		exit(1);
	}

	i = 2;
	pt[i] = em_oalloc(&mm, 20); // 2 block
	if(pt[i] == dummy_ptr || pt[i] == NULL){
		em_printf(EM_LOG_ERROR, "error\n");
		exit(1);
	}

	i = 3;
	pt[i] = em_oalloc(&mm, 20); // 2 block -> fail
	if(pt[i] != NULL){
		em_printf(EM_LOG_ERROR, "error\n");
		exit(1);
	}

	pt[i] = em_oalloc(&mm, 16); // 1 block -> ok
	if(pt[i] == dummy_ptr || pt[i] == NULL){
		em_printf(EM_LOG_ERROR, "error\n");
		exit(1);
	}

	if( 0 != em_omemmng_delete(&mm)){
		em_printf(EM_LOG_ERROR, "error\n");
		exit(1);
	}

	em_printf(EM_LOG_TOP, "oalooc test end: success\n");

	return 0;
}