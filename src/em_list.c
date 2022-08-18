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
#include "em_list.h"
#include "em_mutex.h"
#include "em_print.h"

int em_list_create(em_list_t *li,
				   int num_max_item,
				   void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	if (0 != em_mpool_create(&li->mp, sizeof(em_listitem_t), num_max_item, alloc_func, free_func))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	li->count = 0;
	li->first = NULL;
	li->last = NULL;
	// li->first.next = &li->last;
	// li->last.back = &li->first;
	return 0;
}

int em_list_delete(em_list_t *li)
{
	em_mpool_delete(&li->mp);
	return 0;
}

int em_list_add(em_list_t *li, void *data)
{
	em_listitem_t *newitem;
	if (0 != em_mpool_alloc_block(&li->mp, (void **)&newitem, EM_NO_WAIT))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	// newitem->data = data;
	// newitem->next = &li->last;
	// newitem->back = li->last.back;
	// newitem->back->next = newitem;
	// newitem->next->back = newitem;

	newitem->data = data;
	newitem->next = NULL;
	if (li->first == NULL) //初回
	{
		newitem->back = NULL;
		li->first = newitem;
	}
	else
	{
		newitem->back = li->last;
		li->last->next = newitem;
	}
	li->last = newitem;

	li->count++;
}

int em_list_remove_at(em_list_t *li, uint index)
{
	em_listitem_t *delitem = li->first;
	if (index >= li->count)
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}

	for (int i = 0; i < index; i++)
	{
		delitem = delitem->next;
	}
	// delitem->back->next = delitem->next;
	// delitem->next->back = delitem->back;

	if (delitem->back == NULL) //先頭アイテム
	{
		li->first = delitem->next;
	}
	else
	{
		delitem->back->next = delitem->next;
	}

	if (delitem->next == NULL) //最終アイテム
	{
		li->last = delitem->back;
	}
	else
	{
		delitem->next->back = delitem->back;
	}

	em_mpool_free_block(&li->mp, delitem);
	li->count--;
	return 0;
}
