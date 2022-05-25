#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "em_malloc.h"

int em_set_meminfo_t(em_meminfo_t *minfo,
					 int mem_index, int mem_length, char is_used,
					 em_meminfo_t *next_meminfo,
					 em_meminfo_t *back_meminfo)
{
	minfo->mem_index = mem_index;
	minfo->mem_length = mem_length;
	minfo->is_used = is_used;
	minfo->next_meminfo = next_meminfo;
	minfo->back_meminfo = back_meminfo;
}

int em_create_memmng(em_memmng_t *mm,
					 int mem_total_size,
					 int mem_unit_size)
{
	mm->mem_total_size = mem_total_size;
	mm->mem_unit_size = mem_unit_size;
	em_create_mpool(&mm->mp_used, sizeof(em_meminfo_t), 100);
	em_create_mpool(&mm->mp_free, sizeof(em_meminfo_t), 100);
	mm->memory = malloc(mem_total_size);

	em_meminfo_t *initial_meminfo;
	em_alloc_block(&mm->mp_free, (void **)&initial_meminfo);
	em_set_meminfo_t(&mm->first_meminfo, -1, 0, 1, initial_meminfo, NULL);
	em_set_meminfo_t(&mm->last_meminfo, -2, 0, 1, NULL, initial_meminfo);
	em_set_meminfo_t(initial_meminfo, 0, mem_total_size / mem_unit_size, 0,
					 &mm->last_meminfo, &mm->first_meminfo);
}

int em_delete_memmng(em_memmng_t *mm)
{
	return 0;
}

int em_print_memmng(em_memmng_t *mm)
{
	int total_used = 0;
	int total_free = 0;
	em_meminfo_t *meminfo;
	printf("mem used: ");
	for (int i = 0; i < mm->mp_used.num_used; i++)
	{
		meminfo = (em_meminfo_t *)mm->mp_used.block_ptr[i]->data_ptr;
		total_used += meminfo->mem_length;
		printf("[%d %d] ", meminfo->mem_index, meminfo->mem_length);
	}
	printf("\n    free: ");
	for (int i = 0; i < mm->mp_free.num_used; i++)
	{
		meminfo = (em_meminfo_t *)mm->mp_free.block_ptr[i]->data_ptr;
		total_free += meminfo->mem_length;
		printf("[%d %d] ", meminfo->mem_index, meminfo->mem_length);
	}
	printf("\n    usage: %.1f%% (%d/%d) \n", total_used * 100.0/total_free, total_used, total_free);
	return 0;
}

void *em_malloc(em_memmng_t *mm, int size)
{
	//メモリ単位変換
	int length = size / mm->mem_unit_size;
	if (size % mm->mem_unit_size != 0)
	{
		length++;
	}
	printf("alloc %d (%d)\n", length, size);

	//空きレコード検索
	em_meminfo_t *meminfo_free;
	em_meminfo_t *meminfo_used;
	for (int i = mm->mp_free.num_used - 1; i >= 0; i--)
	{
		meminfo_free = (em_meminfo_t *)mm->mp_free.block_ptr[i]->data_ptr;
		if (meminfo_free->mem_length >= length)
		{
			em_alloc_block(&mm->mp_used, (void **)&meminfo_used);
			meminfo_used->mem_index = meminfo_free->mem_index;
			meminfo_used->mem_length = length;
			meminfo_used->back_meminfo = meminfo_free->back_meminfo;
			meminfo_used->is_used = 1;
			meminfo_free->back_meminfo->next_meminfo = meminfo_used;

			//レコード組み換え
			if (meminfo_free->mem_length == length) // meminfo_free削除
			{
				meminfo_used->next_meminfo = meminfo_free->next_meminfo;
				em_free_block(&mm->mp_free, meminfo_free);
			}
			else
			{
				meminfo_used->next_meminfo = meminfo_free;
				meminfo_free->mem_index += length;
				meminfo_free->mem_length -= length;
				meminfo_free->back_meminfo = meminfo_used;
			}

			return mm->memory + meminfo_used->mem_index * mm->mem_unit_size;
		}
	}

	return NULL;
}

int em_free(em_memmng_t *mm, void *addr)
{
	//メモリ単位変換
	int index = (addr - mm->memory) / mm->mem_unit_size;

	printf("free %d (%p)\n", index, addr);

	//空きレコード検索
	em_meminfo_t *meminfo_used;
	em_meminfo_t *back_meminfo;
	em_meminfo_t *next_meminfo;
	em_meminfo_t *new_meminfo;
	for (int i = 0; i < mm->mp_used.num_used; i++)
	{
		meminfo_used = (em_meminfo_t *)mm->mp_used.block_ptr[i]->data_ptr;
		if (meminfo_used->mem_index == index)
		{
			back_meminfo = meminfo_used->back_meminfo;
			next_meminfo = meminfo_used->next_meminfo;

			if (back_meminfo->is_used)
			{
				if (next_meminfo->is_used) // free管理レコード追加
				{
					em_alloc_block(&mm->mp_free, (void **)&new_meminfo);
					memcpy(new_meminfo, meminfo_used, sizeof(em_meminfo_t));
					new_meminfo->is_used = 0;
					back_meminfo->next_meminfo = new_meminfo;
					next_meminfo->back_meminfo = new_meminfo;
				}
				else // Nextのfree管理に統合
				{
					next_meminfo->mem_index = meminfo_used->mem_index;
					next_meminfo->mem_length += meminfo_used->mem_length;
					back_meminfo->next_meminfo = next_meminfo;
					next_meminfo->back_meminfo = back_meminfo;
				}
			}
			else // Backがfree
			{
				if (next_meminfo->is_used) // Backのfree管理に統合
				{
					back_meminfo->mem_length += meminfo_used->mem_length;
					back_meminfo->next_meminfo = next_meminfo;
					next_meminfo->back_meminfo = back_meminfo;
				}
				else // Nextも含めBackに統合
				{
					back_meminfo->mem_length += meminfo_used->mem_length;
					back_meminfo->mem_length += next_meminfo->mem_length;
					back_meminfo->next_meminfo = next_meminfo->next_meminfo;
					next_meminfo->back_meminfo = back_meminfo->back_meminfo;
					em_free_block(&mm->mp_free, next_meminfo);
				}
			}
			em_free_block(&mm->mp_used, meminfo_used);

			return 0;
		}
	}

	return -1;
}
