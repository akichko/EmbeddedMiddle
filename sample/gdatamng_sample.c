#include <stdio.h>
#include <stdlib.h>
#include "../src/em_gdatamng.h"

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
	// em_blkinfo_t *block;
	em_test_t test_data = {0, 0, 0};
	em_test_t test_data2 = {11, 0, 0};
	em_test_t test_data3 = {11, 1, 2};
	em_test_t read_data;
	int ret;

	int key;
	int *key_ptr;

	em_gdatamng_create(&dm, sizeof(em_test_t), 10,
					   EM_DMNG_KEY_INTEGER, sizeof(int), EM_DMNG_DPLCT_COUNTUP,
					   &malloc, &free);
	em_gdatamng_print(&dm);

	// printf("add id=1,2,3\n");
	for (int i = 0; i < 3; i++)
	{
		test_data.attr1 = i + 10;
		key = i + 1;
		printf("add id=%d %d\n", i + 1, test_data.attr1);
		em_gdatamng_add_data(&dm, &key, &test_data);
	}
	em_gdatamng_print(&dm);

	key = 2;
	ret = em_gdatamng_get_data(&dm, &key, &read_data);
	printf("get_block id=2 -> ret:%d val=%d\n", ret, read_data.attr1);

	key = 5;
	ret = em_gdatamng_get_data(&dm, &key, &read_data);
	printf("get_block id=5 -> ret:%d\n", ret);

	key_ptr = (int*)em_gdatamng_get_key(&dm, &test_data2);
	printf("em_gdatamng_get_id by test_data2 -> ret:%d\n", *key_ptr);

	key_ptr = (int*)em_gdatamng_get_key(&dm, &test_data3);
	printf("em_gdatamng_get_id by test_data3 -> ret addr:%p\n", key_ptr);

	key_ptr = (int*)em_gdatamng_get_key_by_func(&dm, &test_data3, &test_comparator);
	printf("em_gdatamng_get_id_by_func by test_data3 -> ret:%d\n", *key_ptr);

	printf("add id=1\n");
	key = 1;
	em_gdatamng_add_data(&dm, &key, &test_data);
	em_gdatamng_print(&dm);

	printf("del id=2\n");
	key = 2;
	em_gdatamng_remove_data(&dm, &key);
	em_gdatamng_print(&dm);

	key_ptr = (int*)em_gdatamng_get_key(&dm, &test_data2);
	printf("em_gdatamng_get_id by test_data2 -> ret addr:%p\n", key_ptr);

	key_ptr = (int*)em_gdatamng_get_key_by_func(&dm, &test_data3, &test_comparator);
	printf("em_gdatamng_get_id_by_func by test_data3 -> ret addr:%p\n", key_ptr);

	printf("add id=5\n");
	key = 5;
	em_gdatamng_add_data(&dm, &key, &test_data);
	em_gdatamng_print(&dm);

	printf("del id=1\n");
	key = 1;
	em_gdatamng_remove_data(&dm, &key);
	em_gdatamng_print(&dm);

	em_gdatamng_destroy(&dm);
}