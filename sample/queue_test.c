#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../src/em_queue.h"

typedef struct
{
	int attr1;
	int attr2;
	int attr3;
} em_test_t;

int main()
{
	em_queue_t qu;
	em_test_t test_data = {0, 0, 0};
	em_test_t test_buf;
	int ret;

	em_queue_create(&qu, sizeof(em_test_t), 4);
	em_queue_print(&qu);

	if (0 != em_dequeue(&qu, &test_buf, 1000))
	{
		printf("Error!: dequeue\n");
		sleep(1);
	}

	for (int i = 0; i < 3; i++)
	{
		test_data.attr1 = i + 10;
		printf("enqueue val=%d\n", test_data.attr1);
		em_enqueue(&qu, &test_data, 1000);
	}
	em_queue_print(&qu);

	ret = em_dequeue(&qu, &test_buf, 1000);
	printf("dequeue -> ret:%d val=%d\n", ret, test_buf.attr1);

	ret = em_dequeue(&qu, &test_buf, 1000);
	printf("dequeue -> ret:%d val=%d\n", ret, test_buf.attr1);

	ret = em_queue_getnum(&qu, 1000);
	printf("em_queue_getnum -> ret:%d\n", ret);

	em_queue_print(&qu);

	for (int i = 3; i < 7; i++)
	{
		test_data.attr1 = i + 10;
		printf("enqueue val=%d\n", test_data.attr1);
		ret = em_enqueue(&qu, &test_data, 2000);
		if (ret != 0)
		{
			printf("Error!: enqueue\n");
			sleep(1);
		}

		em_queue_print(&qu);
	}

	for (int i = 0; i < 5; i++)
	{
		ret = em_dequeue(&qu, &test_buf, 1000);
		if (ret != 0)
			printf("Error!: dequeue\n");
		else
			printf("dequeue -> ret:%d val=%d\n", ret, test_buf.attr1);
		em_queue_print(&qu);
	}

	em_queue_delete(&qu);
}