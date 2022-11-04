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
#include <memory.h>
#include <string.h>

#include "em_datamng.h"
#include "em_print.h"

int em_datamng_create(em_datamng_t *dm, uint data_size, uint data_num, int duplicate_mode,
					  void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	return em_gdatamng_create(dm, data_size, data_num,
							  EM_DMNG_KEY_INTEGER, sizeof(ulong), duplicate_mode,
							  alloc_func, free_func);
}

int em_datamng_destroy(em_datamng_t *dm)
{
	return em_gdatamng_destroy(dm);
}

int em_datamng_print(em_datamng_t *dm)
{
	return em_gdatamng_print(dm);
}

int em_datamng_add_data(em_datamng_t *dm, ulong id, void *data)
{
	return em_gdatamng_add_data(dm, &id, data);
}

void *em_datamng_get_data_ptr(em_datamng_t *dm, ulong id)
{
	return em_gdatamng_get_data_ptr(dm, &id);
}

int em_datamng_get_dataidx(em_datamng_t *dm, ulong id)
{
	return em_gdatamng_get_dataidx(dm, &id);
}

void *em_datamng_get_dataptr_by_dataidx(em_datamng_t *dm, uint data_idx)
{
	return em_gdatamng_get_dataptr_by_dataidx(dm, data_idx);
}

int em_datamng_get_data(em_datamng_t *dm, ulong id, void *data)
{
	return em_gdatamng_get_data(dm, &id, data);
}

int em_datamng_get_data_cnt(em_datamng_t *dm, ulong id)
{
	return em_gdatamng_get_data_cnt(dm, &id);
}

int em_datamng_remove_data(em_datamng_t *dm, ulong id)
{
	return em_gdatamng_remove_data(dm, &id);
}

ulong em_datamng_get_id(em_datamng_t *dm, void *searchdata)
{
	ulong *ret = (ulong*)em_gdatamng_get_key(dm, searchdata);
	if (ret == NULL)
		return EM_DATAMNG_INVALID_ID;

	return *ret;
}

ulong em_datamng_get_id_by_func(em_datamng_t *dm, void *searchdata,
								char (*comparator)(void *, void *))
{
	ulong *ret = (ulong*)em_gdatamng_get_key_by_func(dm, searchdata, comparator);
	if (ret == NULL)
		return EM_DATAMNG_INVALID_ID;

	return *ret;
}

int em_datamng_get_data_by_func(em_datamng_t *dm, void *searchdata,
								char (*comparator)(void *, void *), void *data)
{
	return em_gdatamng_get_data_by_func(dm, searchdata, comparator, data);
}
