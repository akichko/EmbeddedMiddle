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
#include "em_task.h"
#include "em_time.h"
#include "em_timer.h"
#include "em_malloc.h"
#include "em_command.h"
#include "em_mtxmng.h"
#include "em_semmng.h"
#include "em_system.h"
#include "em_eventflag.h"

int em_sysmng_init(em_sysmng_t *sysmng, em_sysmng_stg_t *setting)
{
	if (0 != em_tick_init(&sysmng->timemng))
	{
		em_printf(EM_LOG_ERROR, "tick init error\n");
		return -1;
	}

	if (0 != em_timermng_init(&sysmng->tmrmng, setting->max_num_mutex,
							  setting->alloc_func, setting->free_func))
	{
		em_printf(EM_LOG_ERROR, "timer init error\n");
		return -1;
	}

	if (0 != em_mtxmng_init(&sysmng->mtxmng, setting->max_num_mutex,
							setting->alloc_func, setting->free_func))
	{
		em_printf(EM_LOG_ERROR, "mtxmng init error\n");
		return -1;
	}

	if (0 != em_semmng_init(&sysmng->semmng, setting->max_num_sem,
							setting->alloc_func, setting->free_func))
	{
		em_printf(EM_LOG_ERROR, "semmng init error\n");
		return -1;
	}

	if (0 != em_evtmng_init(&sysmng->evtmng, setting->max_num_event,
							setting->alloc_func, setting->free_func))
	{
		em_printf(EM_LOG_ERROR, "evtmng init error\n");
		return -1;
	}

	if (0 != em_cmd_init(&sysmng->cmdmng, setting->max_num_cmd,
						 setting->alloc_func, setting->free_func))
	{
		em_printf(EM_LOG_ERROR, "cmdmng init error\n");
		return -1;
	}

	if (0 != em_taskmng_init(&sysmng->tskmng, setting->max_num_task, setting->msgdata_size,
							 setting->alloc_func, setting->free_func))
	{
		em_printf(EM_LOG_ERROR, "tskmng init error\n");
		return -1;
	}

	if (0 != em_evtarray_init(&sysmng->gevents, setting->num_global_event,
							  setting->alloc_func, setting->free_func))
	{
		em_printf(EM_LOG_ERROR, "event array init error\n");
		return -1;
	}

	return 0;
}

int em_sysmng_finalize(em_sysmng_t *sysmng)
{

	if (0 != em_timermng_destroy(&sysmng->tmrmng))
	{
		em_printf(EM_LOG_ERROR, "timer destroy error\n");
		return -1;
	}

	if (0 != em_mtxmng_destroy(&sysmng->mtxmng))
	{
		em_printf(EM_LOG_ERROR, "mtxmng destroy error\n");
		return -1;
	}

	if (0 != em_semmng_destroy(&sysmng->semmng))
	{
		em_printf(EM_LOG_ERROR, "semmng destroy error\n");
		return -1;
	}

	if (0 != em_evtmng_destroy(&sysmng->evtmng))
	{
		em_printf(EM_LOG_ERROR, "evtmng destroy error\n");
		return -1;
	}

	if (0 != em_cmd_destroy(&sysmng->cmdmng))
	{
		em_printf(EM_LOG_ERROR, "cmdmng destroy error\n");
		return -1;
	}

	if (0 != em_taskmng_destroy(&sysmng->tskmng))
	{
		em_printf(EM_LOG_ERROR, "tskmng init error\n");
		return -1;
	}

	if (0 != em_evtarray_destroy(&sysmng->gevents))
	{
		em_printf(EM_LOG_ERROR, "event array destroy error\n");
		return -1;
	}

	return 0;
}