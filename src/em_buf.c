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
#include <string.h>

#include "em_buf.h"
#include "em_print.h"

int em_buf_init(em_buf_t *bf, uint buf_capacity,
				void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	bf->buf_capacity = buf_capacity;
	bf->data_size = 0;
	bf->data = (char *)alloc_func(buf_capacity+1);
	bf->data[0] = '\0';
	bf->data[buf_capacity] = '\0';
	bf->free_func = free_func;

	return 0;
}

int em_buf_clear(em_buf_t *bf)
{
	bf->data_size = 0;
	bf->data[0] = '\0';
	return 0;
}

int em_buf_destroy(em_buf_t *bf)
{
	bf->free_func(bf->data);
	bf->data = NULL;
	return 0;
}

int em_buf_get_remain_size(em_buf_t *bf)
{
	if (bf == NULL)
	{
		em_printf(EM_LOG_ERROR, "not init\n");
		return -1;
	}
	if (bf->data == NULL)
	{
		em_printf(EM_LOG_ERROR, "not init\n");
		return -1;
	}

	return bf->buf_capacity - bf->data_size;
}

int em_buf_append(em_buf_t *bf, const char *append_data, int length)
{
	if (bf == NULL || bf->data == NULL)
	{
		em_printf(EM_LOG_ERROR, "not init\n");
		return -1;
	}
	if (bf->data_size + length > bf->buf_capacity)
	{
		em_printf(EM_LOG_ERROR, "bffer overflow %ld/%ld\n", bf->data_size + length, bf->buf_capacity);
		return -2;
	}

	memcpy(bf->data + bf->data_size, append_data, length);
	bf->data_size += length;

	return 0;
}
