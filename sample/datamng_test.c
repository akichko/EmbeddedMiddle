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
	em_blkinfo_t *block;
	em_test_t test_data = {0, 0, 0};
	int ret;

	em_datamng_create(&dm, sizeof(em_test_t), 10);
	em_datamng_print(&dm);

	//printf("add id=1,2,3\n");
	for (int i = 0; i < 3; i++)
	{
		test_data.attr1 = i + 10;
		printf("add id=%d\n", i+1);
		em_datamng_add_data(&dm, i + 1, &test_data);
	}
	em_datamng_print(&dm);

	ret = _em_datamng_get_blockinfo(&dm, 2, &block);
	printf("get_block id=2 -> ret:%d val=%d\n", ret,
		   ((em_test_t *)block->data_ptr)->attr1);

	ret = _em_datamng_get_blockinfo(&dm, 5, &block);
	printf("get_block id=5 -> ret:%d\n", ret);

	printf("add id=1\n");
	em_datamng_add_data(&dm, 1, &test_data);
	em_datamng_print(&dm);

	printf("del id=2\n");
	em_datamng_remove_data(&dm, 2);
	em_datamng_print(&dm);

	printf("add id=5\n");
	em_datamng_add_data(&dm, 5, &test_data);
	em_datamng_print(&dm);

	printf("del id=1\n");
	em_datamng_remove_data(&dm, 1);
	em_datamng_print(&dm);

	em_datamng_delete(&dm);
}