#ifndef __EM_OALLOC_H__
#define __EM_OALLOC_H__

typedef struct
{
	int is_malloc;
	int mem_unit_size;
	int mem_total_bnum;
	int next_alloc_index;
	void *memory;
} em_omemmng_t;

int em_omemmng_create(em_omemmng_t *mm,
					   int mem_total_size,
					   int mem_unit_size,
					   void *memory);

int em_omemmng_delete(em_omemmng_t *mm);

int em_omemmng_print(em_omemmng_t *mm);

void *em_oalloc(em_omemmng_t *mm,
				  int size);

#endif //__EM_OALLOC_H__
