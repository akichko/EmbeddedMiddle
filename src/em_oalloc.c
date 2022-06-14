#include <stdio.h>
#include <stdlib.h>
#include "em_oalloc.h"
#include "em_cmndefs.h"

int em_omemmng_create(em_omemmng_t *mm,
					   int mem_total_size,
					   int mem_unit_size,
					   void *memory)
{
	mm->mem_unit_size = mem_unit_size;
	mm->mem_total_bnum = mem_total_size / mem_unit_size;
	mm->next_alloc_index = 0;
	if (memory != NULL)
	{
		mm->is_malloc = 0;
		mm->memory = memory;
	}
	else
	{
		mm->is_malloc = 1;
		mm->memory = malloc(mem_total_size);
	}
}

int em_omemmng_delete(em_omemmng_t *mm)
{
	if (mm->is_malloc)
	{
		free(mm->memory);
	}

	return 0;
}

int em_omemmng_print(em_omemmng_t *mm)
{
	int used = mm->next_alloc_index;
	int total = mm->mem_total_bnum;

	printf("mem usage: %.1f%% (%d/%d) \n",
		   used * 100.0 / total,
		   used, total);
	return 0;
}

void *em_oalloc(em_omemmng_t *mm, int size)
{
	void *ret;
	//メモリ単位変換
	int blength = size / mm->mem_unit_size;
	if (size - blength * mm->mem_unit_size > 0)
	{
		blength++;
	}
	em_printf(EM_LOG_DEBUG, "alloc %d (%ld)\n", blength, size);

	if (blength < mm->mem_total_bnum - mm->next_alloc_index)
	{
		ret = mm->memory + mm->next_alloc_index * mm->mem_unit_size;
		mm->next_alloc_index += blength;
		return ret;
	}

	em_printf(EM_LOG_ERROR, "allocation failed\n");
	return NULL;
}
