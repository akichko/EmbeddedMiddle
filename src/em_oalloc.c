/*============================================================================
MIT License

Copyright (c) 2022 akichko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "em_oalloc.h"
#include "em_cmndefs.h"

int em_omemmng_create(em_omemmng_t *mm,
					  int mem_total_size,
					  int mem_unit_size,
					  void *memory,
					  void *(*alloc_func)(size_t),
					  void (*free_func)(void *))
{
	mm->free_func = free_func;
	mm->mem_unit_size = mem_unit_size;
	mm->mem_total_bnum = mem_total_size / mem_unit_size;
	mm->next_alloc_index = 0;
	if (memory != NULL)
	{
		mm->is_malloc = FALSE;
		mm->memory = memory;
	}
	else
	{
		mm->is_malloc = TRUE;
		mm->memory = alloc_func(mem_total_size);
	}
	return 0;
}

int em_omemmng_delete(em_omemmng_t *mm)
{
	if (mm->is_malloc)
	{
		mm->free_func(mm->memory);
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

void *em_oalloc(em_omemmng_t *mm, size_t size)
{
	void *ret;
	//メモリ単位変換
	int blength = size / mm->mem_unit_size;
	if (size - blength * mm->mem_unit_size > 0)
	{
		blength++;
	}
	em_printf(EM_LOG_DEBUG, "alloc %d (%ld)\n", blength, size);

	if (blength <= mm->mem_total_bnum - mm->next_alloc_index)
	{
		ret = (char *)mm->memory + mm->next_alloc_index * mm->mem_unit_size;
		mm->next_alloc_index += blength;
		return ret;
	}

	em_printf(EM_LOG_ERROR, "allocation failed\n");
	return NULL;
}
