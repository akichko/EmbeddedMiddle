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
