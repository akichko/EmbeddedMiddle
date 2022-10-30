#include <stdio.h>
#include <stdlib.h>

#include "../src/em_command.h"

em_cmdmng_t cm;

void exit_cmd_func(int argc, char **argv)
{
	printf("command stop\n");
	em_cmd_stop(&cm);
}

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
	{"exit", exit_cmd_func},
	{"test", test_cmd_func},
	{"version", test_cmd_version}};

int main()
{
	// int ret;

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

	char cmd1[16] = "test aa bb cc";
	printf("$ test cmd %s\n", cmd1);
	if (0 != em_cmd_exec(&cm, cmd1))
	{
		printf(" -> em_cmd_exec error\n");
	}

	char cmd2[16] = "tes aa bb cc";
	printf("$ test cmd %s\n", cmd2);
	if (0 != em_cmd_exec(&cm, cmd2))
	{
		printf(" -> em_cmd_exec error\n");
	}

	em_cmd_start(&cm);
	return 0;
}
