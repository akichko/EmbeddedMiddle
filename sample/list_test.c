#include <stdio.h>
#include <stdlib.h>
#include "../src/em_list.h"

void list_print(em_list_t *li)
{
	em_listitem_t *item = li->first;
	printf("list: count=%d", li->count);

	int index = 0;
	while (item != NULL)
	{
		printf(" [%d:%s]", index++, (char *)item->data);
		item = item->next;
	}
	printf("\n");
}

int main()
{
	char *test_data[] = {"aa", "bb", "cc", "dd"};

	em_list_t li;
	int ret;

	em_list_create(&li, 5, &malloc, &free);
	list_print(&li);

	for (int i = 0; i < 3; i++)
	{
		em_list_add(&li, test_data[i]);
	}
	list_print(&li);

	ret = em_list_remove_at(&li, 1);
	printf("del idx=1 -> ret=%d\n",ret);
	list_print(&li);

	ret = em_list_remove_at(&li, 0);
	printf("del idx=0 -> ret=%d\n",ret);
	list_print(&li);

	ret = em_list_remove_at(&li, 0);
	printf("del idx=0 -> ret=%d\n",ret);
	list_print(&li);

	ret = em_list_remove_at(&li, 0);
	printf("del idx=0 -> ret=%d\n",ret);
	list_print(&li);

	for (int i = 0; i < 6; i++)
	{
		em_list_add(&li, test_data[0]);
		list_print(&li);
	}

	em_list_delete(&li);
}