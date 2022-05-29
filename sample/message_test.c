#include <stdio.h>
#include "../src/em_message.h"
#include "../src/em_timer.h"

em_msgmng_t mm;

void timer_func(union sigval arg)
{
	static int i = 0;
	int ret;

	em_msg_t msg;
	msg.msg_type = 111;
	msg.taskid_to = 5;
	msg.data[0] = i++;
	if (0 != em_msg_send(&mm, msg, 100))
	{
		printf("msg send to taskid=5 failed\n");
	}
	msg.taskid_to = 6;
	if (0 != em_msg_send(&mm, msg, 100))
	{
		printf("msg send to taskid=6 failed\n");
	}
}

int init()
{
	timer_t timer_id;
	int ret = 0;
	em_msgmng_init(&mm, 3);
	em_msgmng_add_mqueue(&mm, 5, 5);
	em_msgmng_add_mqueue(&mm, 6, 5);
	// em_queue_print(&mqueue);

	ret = em_timer_create(&timer_id, timer_func, 1000);
	if (ret != 0)
	{
		printf("em_timer_create error!!\n");
		return -1;
	}
	printf("[Main] timer created\n");
}

int main()
{
	int ret;
	em_msg_t msg;

	init();

	while (1)
	{
		ret = em_msg_resv(&mm, &msg, 5, EM_NO_TIMEOUT);
		if (ret == 0)
			printf("[Main] dequeue -> ret:%d id=%d->%d mtype=%d val=%d\n", ret, msg.taskid_from, msg.taskid_to, msg.msg_type, msg.data[0]);
		else
			printf("[Main] dequeue -> ret:%d timeout\n", ret);
	}
	// em_queue_delete(&mqueue);
}
