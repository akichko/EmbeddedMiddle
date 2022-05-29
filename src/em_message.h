#ifndef __EM_MESSAGE_H__
#define __EM_MESSAGE_H__

#include "em_queue.h"
#include "em_datamng.h"

typedef struct
{
	int msg_type;
	short taskid_to;
	short taskid_from;
	int data[6];
} em_msg_t;

typedef struct
{
	 int mqueue_num_max;
	 int mqueue_num_used;
	em_queue_t *mqueue;
	em_datamng_t taskid_queue_mng;
} em_msgmng_t;

int em_msgmng_init(em_msgmng_t *mm,
				   int mqueue_num);

int em_msgmng_add_mqueue(em_msgmng_t *mm,
						 int id, int mqueue_length);

em_queue_t *em_msgmng_get_queue(em_msgmng_t *mm,
								int taskid);

int em_msg_send(em_msgmng_t *mm,
				em_msg_t message,
				 int timeout_ms);

int em_msg_resv(em_msgmng_t *mm,
				em_msg_t *message,
				 int taskid,
				 int timeout_ms);

#endif //__EM_MESSAGE_H__
