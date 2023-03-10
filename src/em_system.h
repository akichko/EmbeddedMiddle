#ifndef __EM_SYSTEM_H__
#define __EM_SYSTEM_H__

#include "em_task.h"
#include "em_time.h"
#include "em_timer.h"
#include "em_malloc.h"
#include "em_command.h"
#include "em_mtxmng.h"
#include "em_semmng.h"
#include "em_eventflag.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct tag_em_sysmng
{
	em_timemng_t timemng;
	em_memmng_t memmng;
	em_timermng_t tmrmng;
	em_mtxmng_t mtxmng;
	em_semmng_t semmng;
	em_evtmng_t evtmng;
	em_taskmng_t tskmng;
	em_cmdmng_t cmdmng;
	em_eventflg_t ef;
} em_sysmng_t;

typedef struct tag_em_sysmng_stg
{
	int max_num_mutex;
	int max_num_sem;
	int max_num_event;
	int max_num_timer;
	int max_num_cmd;
	int max_num_task;
	int size_msgdata;
	int mem_block_size;
	int mem_block_num;
	int mem_alloc_num;
	char *mem_static;
	void *(*alloc_func)(size_t);
	void (*free_func)(void *);
} em_sysmng_stg_t;

int em_sysmng_init(em_sysmng_t *sysmng,
				   em_sysmng_stg_t *setting);

int em_sysmng_finalize(em_sysmng_t *sysmng);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_SYSTEM_H__
