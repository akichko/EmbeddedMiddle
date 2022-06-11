#include <stdio.h>
#include <stdlib.h>
#include "../src/em_datamng.h"

typedef struct
{
	int attr1;
	int attr2;
	int attr3;
} em_test_t;

char test_comparator(void *a, void *b)
{
	if (((em_test_t *)a)->attr1 == ((em_test_t *)b)->attr1)
	{
		return 1;
	}
	return 0;
}

int main()
{
	em_datamng_t dm;
	em_blkinfo_t *block;
	em_test_t test_data = {0, 0, 0};
	em_test_t test_data2 = {11, 0, 0};
	em_test_t test_data3 = {11, 1, 2};
	int ret;
	long lret;

	em_datamng_create(&dm, sizeof(em_test_t), 10, &malloc, &free);
	em_datamng_print(&dm);

	// printf("add id=1,2,3\n");
	for (int i = 0; i < 3; i++)
	{
		test_data.attr1 = i + 10;
		printf("add id=%d\n", i + 1);
		em_datamng_add_data(&dm, i + 1, &test_data);
	}
	em_datamng_print(&dm);

	ret = _em_datamng_get_blockinfo(&dm, 2, &block);
	printf("get_block id=2 -> ret:%d val=%d\n", ret,
		   ((em_test_t *)block->data_ptr)->attr1);

	ret = _em_datamng_get_blockinfo(&dm, 5, &block);
	printf("get_block id=5 -> ret:%d\n", ret);

	lret = em_datamng_get_id(&dm, &test_data2);
	printf("em_datamng_get_id by test_data2 -> ret:%ld\n", lret);

	lret = em_datamng_get_id(&dm, &test_data3);
	printf("em_datamng_get_id by test_data3 -> ret:%ld\n", lret);

	lret = em_datamng_get_id_by_func(&dm, &test_data3, &test_comparator);
	printf("em_datamng_get_id_by_func by test_data3 -> ret:%ld\n", lret);

	printf("add id=1\n");
	em_datamng_add_data(&dm, 1, &test_data);
	em_datamng_print(&dm);

	printf("del id=2\n");
	em_datamng_remove_data(&dm, 2);
	em_datamng_print(&dm);

	lret = em_datamng_get_id(&dm, &test_data2);
	printf("em_datamng_get_id by test_data2 -> ret:%ld\n", lret);

	lret = em_datamng_get_id_by_func(&dm, &test_data3, &test_comparator);
	printf("em_datamng_get_id_by_func by test_data3 -> ret:%ld\n", lret);

	printf("add id=5\n");
	em_datamng_add_data(&dm, 5, &test_data);
	em_datamng_print(&dm);

	printf("del id=1\n");
	em_datamng_remove_data(&dm, 1);
	em_datamng_print(&dm);

	em_datamng_delete(&dm);
}