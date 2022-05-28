#include <stdio.h>
#include "../src/em_datamng.h"

typedef struct
{
	int attr1;
	int attr2;
	int attr3;
} em_test_t;

int main()
{
	em_datamng_t dm;
	em_blockmng_t *block;
	em_test_t test_data = {0, 0, 0};
	int ret;

	em_create_datamng(&dm, sizeof(em_test_t), 10);
	em_print_datamng(&dm);

	//printf("add id=1,2,3\n");
	for (int i = 0; i < 3; i++)
	{
		test_data.attr1 = i + 10;
		printf("add id=%d\n", i+1);
		em_set_data(&dm, &test_data, i + 1);
	}
	em_print_datamng(&dm);

	ret = em_get_block(&dm, 2, &block);
	printf("get_block id=2 -> ret:%d val=%d\n", ret,
		   ((em_test_t *)block->data_ptr)->attr1);

	ret = em_get_block(&dm, 5, &block);
	printf("get_block id=5 -> ret:%d\n", ret);

	printf("add id=1\n");
	em_set_data(&dm, &test_data, 1);
	em_print_datamng(&dm);

	printf("del id=2\n");
	em_del_block(&dm, 2);
	em_print_datamng(&dm);

	printf("add id=5\n");
	em_set_data(&dm, &test_data, 5);
	em_print_datamng(&dm);

	printf("del id=1\n");
	em_del_block(&dm, 1);
	em_print_datamng(&dm);

	em_delete_datamng(&dm);
}