#include <stdio.h>
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

	em_create_queue(&qu, sizeof(em_test_t), 4);
	em_print_queue(&qu);

	for (int i = 0; i < 3; i++)
	{
		test_data.attr1 = i + 10;
		printf("enqueue val=%d\n", test_data.attr1);
		em_enqueue(&qu, &test_data);
	}
	em_print_queue(&qu);

	ret = em_dequeue(&qu, &test_buf);
	printf("dequeue -> ret:%d val=%d\n", ret, test_buf.attr1);

	ret = em_dequeue(&qu, &test_buf);
	printf("dequeue -> ret:%d val=%d\n", ret, test_buf.attr1);

	em_print_queue(&qu);

	for (int i = 3; i < 7; i++)
	{
		test_data.attr1 = i + 10;
		printf("enqueue val=%d\n", test_data.attr1);
		ret = em_enqueue(&qu, &test_data);
		if (ret != 0)
			printf("Error!: enqueue\n");

		em_print_queue(&qu);
	}

	for (int i = 0; i < 5; i++)
	{
		ret = em_dequeue(&qu, &test_buf);
		if (ret != 0)
			printf("Error!: dequeue\n");
		else
			printf("dequeue -> ret:%d val=%d\n", ret, test_buf.attr1);
		em_print_queue(&qu);
	}

	em_delete_queue(&qu);
}