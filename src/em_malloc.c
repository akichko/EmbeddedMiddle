#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "em_malloc.h"
#include "em_print.h"
#include "em_time.h"

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

int em_memmng_create(em_memmng_t *mm,
					 int mem_total_size,
					 int mem_unit_size,
					 int alloc_max_num)
{
	// mem_unit_sizeが２の階乗
	if (mem_unit_size <= 0 || (mem_unit_size & (mem_unit_size - 1) != 0))
	{
		em_printf(EM_LOG_ERROR, "Error: mem_unit_size shoud be 2 ^ x\n");
		return -1;
	}

	mm->mem_total_size = mem_total_size;
	mm->mem_unit_size = mem_unit_size;
	mm->mem_total_bnum = mem_total_size / mem_unit_size;
	mm->mem_unit_bshift = log2((double)mem_unit_size);
	mm->mem_used_bsize = 0;
	em_mpool_create(&mm->mp_used, sizeof(em_meminfo_t), alloc_max_num, &malloc, &free);
	em_mpool_create(&mm->mp_free, sizeof(em_meminfo_t), alloc_max_num, &malloc, &free);
	mm->memory = malloc(mem_total_size);
	mm->minfo_ptr = (em_meminfo_t **)malloc(sizeof(em_meminfo_t *) * mm->mem_total_bnum);
	em_mutex_init(&mm->mutex);

	pthread_cond_init(&mm->cond, NULL);
	em_mutex_init(&mm->cond_mutex);

	em_meminfo_t *initial_meminfo;
	if (0 != em_mpool_alloc_block(&mm->mp_free, (void **)&initial_meminfo, 0))
	{
		em_printf(EM_LOG_ERROR, "Error: em_create_memmng");
	}
	em_set_meminfo_t(&mm->first_meminfo, -1, 0, 1, initial_meminfo, NULL);
	em_set_meminfo_t(&mm->last_meminfo, -2, 0, 1, NULL, initial_meminfo);
	em_set_meminfo_t(initial_meminfo, 0, mm->mem_total_bnum, 0,
					 &mm->last_meminfo, &mm->first_meminfo);
}

int em_memmng_delete(em_memmng_t *mm)
{
	free(mm->memory);
	free(mm->minfo_ptr);
	em_mutex_destroy(&mm->mutex);
	em_mutex_destroy(&mm->cond_mutex);
	pthread_cond_destroy(&mm->cond);

	return 0;
}

int em_memmng_print(em_memmng_t *mm)
{
	int total_used = mm->mem_used_bsize;
	// int total_free = (mm->mem_total_size >> mm->mem_unit_bshift) - mm->mem_used_bsize;
	int total = mm->mem_total_bnum;
	em_meminfo_t *meminfo;
	printf("mem used: ");
	for (int i = 0; i < mm->mp_used.num_used; i++)
	{
		meminfo = (em_meminfo_t *)mm->mp_used.block_ptr[i]->data_ptr;
		// total_used += meminfo->mem_length;
		printf("[%d %d] ", meminfo->mem_index, meminfo->mem_length);
	}
	printf("\n    free: ");
	for (int i = 0; i < mm->mp_free.num_used; i++)
	{
		meminfo = (em_meminfo_t *)mm->mp_free.block_ptr[i]->data_ptr;
		// total_free += meminfo->mem_length;
		printf("[%d %d] ", meminfo->mem_index, meminfo->mem_length);
	}
	printf("\n    usage: %.1f%% (%d/%d) \n",
		   total_used * 100.0 / total,
		   total_used, total);
	return 0;
}

int _em_malloc(em_memmng_t *mm, size_t size, void **mem)
{
	if (size <= 0)
	{
		*mem = NULL;
		return -1;
	}
	//メモリ単位変換
	int blength = size >> mm->mem_unit_bshift;
	if ((size & (mm->mem_unit_size - 1)) != 0)
	{
		blength++;
	}
	em_printf(EM_LOG_TRACE, "alloc %d (%ld)\n", blength, size);

	//空きレコード検索
	em_meminfo_t *meminfo_free;
	em_meminfo_t *meminfo_new;
	for (int i = mm->mp_free.num_used - 1; i >= 0; i--)
	{
		meminfo_free = (em_meminfo_t *)mm->mp_free.block_ptr[i]->data_ptr;
		if (meminfo_free->mem_length >= blength)
		{
			mm->mem_used_bsize += blength;
			if (0 != em_mpool_alloc_block(&mm->mp_used, (void **)&meminfo_new, 0))
			{
				em_printf(EM_LOG_ERROR, "Fatal error! alloc num max\n");
				*mem = NULL;
				return -1;
			}
			meminfo_new->mem_index = meminfo_free->mem_index;
			meminfo_new->mem_length = blength;
			meminfo_new->back_meminfo = meminfo_free->back_meminfo;
			meminfo_new->is_used = 1;
			meminfo_free->back_meminfo->next_meminfo = meminfo_new;

			//レコード組み換え
			if (meminfo_free->mem_length == blength) // meminfo_free削除
			{
				meminfo_new->next_meminfo = meminfo_free->next_meminfo;
				em_mpool_free_block(&mm->mp_free, meminfo_free);
			}
			else
			{
				meminfo_new->next_meminfo = meminfo_free;
				meminfo_free->mem_index += blength;
				meminfo_free->mem_length -= blength;
				meminfo_free->back_meminfo = meminfo_new;
			}

			mm->minfo_ptr[meminfo_new->mem_index] = meminfo_new;

			*mem = mm->memory + (meminfo_new->mem_index << mm->mem_unit_bshift);
			return 0;
		}
	}
	// em_printf(EM_LOG_DEBUG, "allocation failed\n");
	*mem = NULL;
	return -1;
}

void *em_malloc(em_memmng_t *mm, size_t size)
{
	void *ret;
	em_mutex_lock(&mm->mutex, EM_NO_TIMEOUT);
	int malloc_ret = _em_malloc(mm, size, &ret);
	em_mutex_unlock(&mm->mutex);

	if (malloc_ret != 0 || ret == NULL)
	{
		em_printf(EM_LOG_DEBUG, "allocation failed\n");
	}
	return ret;
}

void *em_trymalloc(em_memmng_t *mm, size_t size, int timeout_ms)
{
	void *ret;
	struct timespec start_ts = em_get_timestamp();
	struct timespec timeout_ts = em_get_offset_timestamp(timeout_ms);
	struct timespec passed_ts;
	int passed_ms;
	int malloc_ret;

	while (1)
	{
		em_mutex_lock(&mm->mutex, EM_NO_TIMEOUT);
		malloc_ret = _em_malloc(mm, size, &ret);

		if (malloc_ret == 0)
		{
			em_mutex_unlock(&mm->mutex);
			break;
		}
		if (timeout_ms == EM_NO_WAIT)
		{
			em_mutex_unlock(&mm->mutex);
			em_printf(EM_LOG_DEBUG, "allocation failed\n");
			break;
		}

		if (timeout_ms == EM_NO_TIMEOUT)
			timeout_ts = em_get_offset_timestamp(10000);

		pthread_mutex_lock(&mm->cond_mutex.mtx);
		em_mutex_unlock(&mm->mutex); // free検知不可区間をつくらないため
		pthread_cond_timedwait(&mm->cond, &mm->cond_mutex.mtx, &timeout_ts);
		pthread_mutex_unlock(&mm->cond_mutex.mtx);

		passed_ts = em_timespec_sub(em_get_timestamp(), start_ts);
		passed_ms = em_calc_timespec2ms(passed_ts);
		em_printf(EM_LOG_TRACE, "passed ms = %d\n", passed_ms);
		if (passed_ms >= timeout_ms)
		{
			em_printf(EM_LOG_DEBUG, "allocation timeout\n");
			break;
		}
	}
	return ret;
}

void em_free(em_memmng_t *mm, void *addr)
{
	//メモリ単位変換
	int index = (addr - mm->memory) >> mm->mem_unit_bshift;
	em_printf(EM_LOG_TRACE, "free %d (%p)\n", index, addr);

	em_mutex_lock(&mm->mutex, EM_NO_TIMEOUT);

	em_meminfo_t *back_meminfo;
	em_meminfo_t *next_meminfo;
	em_meminfo_t *new_meminfo;
	em_meminfo_t *meminfo_del = (em_meminfo_t *)mm->minfo_ptr[index];

	back_meminfo = meminfo_del->back_meminfo;
	next_meminfo = meminfo_del->next_meminfo;

	if (back_meminfo->is_used)
	{
		if (next_meminfo->is_used) // free管理レコード追加
		{
			if (0 != em_mpool_alloc_block(&mm->mp_free, (void **)&new_meminfo, 0))
			{
				em_printf(EM_LOG_ERROR, "error");
			}
			memcpy(new_meminfo, meminfo_del, sizeof(em_meminfo_t));
			new_meminfo->is_used = 0;
			back_meminfo->next_meminfo = new_meminfo;
			next_meminfo->back_meminfo = new_meminfo;
		}
		else // Nextのfree管理に統合
		{
			next_meminfo->mem_index = meminfo_del->mem_index;
			next_meminfo->mem_length += meminfo_del->mem_length;
			back_meminfo->next_meminfo = next_meminfo;
			next_meminfo->back_meminfo = back_meminfo;
		}
	}
	else // Backがfree
	{
		if (next_meminfo->is_used) // Backのfree管理に統合
		{
			back_meminfo->mem_length += meminfo_del->mem_length;
			back_meminfo->next_meminfo = next_meminfo;
			next_meminfo->back_meminfo = back_meminfo;
		}
		else // Nextも含めBackに統合
		{
			back_meminfo->mem_length += meminfo_del->mem_length;
			back_meminfo->mem_length += next_meminfo->mem_length;
			back_meminfo->next_meminfo = next_meminfo->next_meminfo;
			next_meminfo->next_meminfo->back_meminfo = back_meminfo;
			em_mpool_free_block(&mm->mp_free, next_meminfo);
		}
	}
	em_mpool_free_block(&mm->mp_used, meminfo_del);

	mm->mem_used_bsize -= meminfo_del->mem_length;

	pthread_mutex_lock(&mm->cond_mutex.mtx);
	pthread_cond_broadcast(&mm->cond);
	pthread_mutex_unlock(&mm->cond_mutex.mtx);

	em_mutex_unlock(&mm->mutex);
}
