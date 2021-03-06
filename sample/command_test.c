#include <stdio.h>
#include <stdlib.h>

#include "../src/em_command.h"

void test_cmd_func(int argc, char **argv)
{
	if (argc == 1)
	{
		printf("testfunc Error: no argument!\n");
	}
	else
	{
		printf("testfunc [%s]\n", argv[1]);
	}
}

void test_cmd_version(int argc, char **argv)
{
	printf("version: 1.00\n");
}

em_cmdsetting_t testcmdset[] = {
	{1, "test", test_cmd_func},
	{2, "version", test_cmd_version}};

int main()
{
	em_cmdmng_t cm;
	//int ret;

	if (0 != em_cmd_init(&cm, 5, &malloc, &free))
	{
		printf("em_command_init error!!\n");
		exit(1);
	}

	int cmdnum = sizeof(testcmdset) / sizeof(em_cmdsetting_t);
	for (int i = 0; i < cmdnum; i++)
	{
		printf("regist command '%s'\n", testcmdset[i].cmd_name);
		if (0 != em_cmd_regist(&cm, &testcmdset[i]))
		{
			printf("regist error\n");
			// exit(2);
		}
	}

	em_cmd_start(&cm);
	return 0;
}
