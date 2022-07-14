#ifndef EM_SEMMNG_H
#define EM_SEMMNG_H

#include "em_mempool.h"

typedef void *em_semp_t;

typedef struct
{
	em_mpool_t mp_semaphore;
	void (*free_func)(void *);
} em_semmng_t;

int em_semmng_init(em_semmng_t *smm,
				   int max_sem_num,
				   void *(*allc_func)(size_t),
				   void (*free_func)(void *));

em_semp_t em_semmng_factory(em_semmng_t *smm,
							int value);

int em_semmng_dispose(em_semmng_t *smm,
					  em_semp_t sem_p);

int em_semmng_wait(em_semp_t sem_p,
				   int timeout_ms);

int em_semmng_post(em_semp_t sem_p);

int em_semmng_semp2id(em_semmng_t *smm,
					  em_semp_t sem_p);

em_semp_t em_semmng_semid2p(em_semmng_t *smm,
							int sem_id);

#endif // EM_SEMMNG_H
