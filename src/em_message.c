#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "em_message.h"

int em_msgmng_init(em_msgmng_t *mm, int mqueue_num)
{
	mm->mqueue = (em_queue_t *)malloc(sizeof(em_queue_t) * mqueue_num);

	mm->mqueue_num_max = mqueue_num;
	mm->mqueue_num_used = 0;

	em_datamng_create(&mm->taskid_queue_mng, sizeof(em_queue_t *), mqueue_num);
	return 0;
}

int em_msgmng_add_mqueue(em_msgmng_t *mm, int id, int mqueue_length)
{
	em_queue_t *qu = &mm->mqueue[mm->mqueue_num_used];
	em_queue_create(qu, sizeof(em_msg_t), mqueue_length);

	em_datamng_add_data(&mm->taskid_queue_mng, id, (void *)&qu);
	mm->mqueue_num_used++;
}

em_queue_t *em_msgmng_get_queue(em_msgmng_t *mm, int taskid)
{
	int ret;
	em_queue_t *mqueue;
	ret = em_datamng_get_data(&mm->taskid_queue_mng, taskid, (void *)&mqueue);
	if (ret != 0)
		return NULL;

	return mqueue;
}

int em_msg_send(em_msgmng_t *mm, em_msg_t message, int timeout_ms)
{
	int ret;
	int taskid_to = message.taskid_to;
	em_queue_t *mqueue = em_msgmng_get_queue(mm, taskid_to);
	if (mqueue == NULL)
	{
		return -1;
	}
	if (0 != em_enqueue(mqueue, &message, timeout_ms))
	{
		return -1;
	}

	return 0;
}

int em_msg_resv(em_msgmng_t *mm, em_msg_t *message, int taskid, int timeout_ms)
{
	int taskid_to = taskid;
	em_queue_t *mqueue = em_msgmng_get_queue(mm, taskid_to);
	em_dequeue(mqueue, message, timeout_ms);

	return 0;
}
