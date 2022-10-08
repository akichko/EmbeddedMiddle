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

typedef struct
{
	em_timemng_t timemng;
	em_timermng_t tmrmng;
	em_mtxmng_t mtxmng;
	em_semmng_t semmng;
	em_evtmng_t evtmng;
	// em_memmng_t memmng;
	em_taskmng_t tskmng;
	em_cmdmng_t cmdmng;
	em_evtarray_t gevents;
} em_sysmng_t;

typedef struct
{
	int max_num_mutex;
	int max_num_sem;
	int max_num_event;
	int max_num_timer;
	int max_num_cmd;
	int max_num_task;
	int max_alloc_num;
	int mem_total_size;
	int mem_unit_size;
	int msgdata_size;
	int num_global_event;
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
