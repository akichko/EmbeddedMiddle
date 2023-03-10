#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../src/em_command.h"

em_cmdmng_t cm;

int exit_cmd_func(int argc, char **argv)
{
	printf("command stop\n");
	em_cmd_stop(&cm);
	return 0;
}

int test_cmd_func(int argc, char **argv)
{
	if (argc == 1)
	{
		printf("testfunc Error: no argument!\n");
	}
	else
	{
		printf("testfunc [%s]\n", argv[1]);
	}
	return 0;
}

int sleep_cmd_func(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: sleep [time milliseconds]\n");
		return -1;
	}
	sleep(atoi(argv[1]));
	return 0;
}

int test_cmd_version(int argc, char **argv)
{
	printf("version: 1.00\n");
	return 0;
}

void *cmd_task_background(void *arg)
{
	em_cmd_start_bg(&cm);
	return NULL;
}

em_cmdsetting_t testcmdset[] = {
	{"exit", exit_cmd_func},
	{"test", test_cmd_func},
	{"sleep", sleep_cmd_func},
	{"version", test_cmd_version}};

int main()
{
	pthread_t thread;
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
		if (0 != em_cmd_register(&cm, &testcmdset[i]))
		{
			printf("regist error\n");
			// exit(2);
		}
	}

	pthread_create(&thread, NULL, &cmd_task_background, NULL);

	char cmd1[16] = "test aa bb cc";
	printf("$ test cmd '%s'\n", cmd1);
	if (0 != em_cmd_exec_string(&cm, cmd1))
	{
		printf(" -> em_cmd_exec error\n");
	}

	char bgcmd1[16] = "async sleep 5";
	printf("$ test cmd '%s'\n", bgcmd1);
	em_cmd_exec_string(&cm, bgcmd1);

	char bgcmd2[16] = "async version";
	printf("$ test cmd '%s' * 5\n", bgcmd2);
	em_cmd_exec_string(&cm, bgcmd2);
	em_cmd_exec_string(&cm, bgcmd2);
	em_cmd_exec_string(&cm, bgcmd2);
	em_cmd_exec_string(&cm, bgcmd2);
	em_cmd_exec_string(&cm, bgcmd2);

	char cmd2[16] = "tes aa bb cc";
	printf("$ test cmd '%s'\n", cmd2);
	if (0 != em_cmd_exec_string(&cm, cmd2))
	{
		printf(" -> em_cmd_exec error\n");
	}

	em_cmd_start(&cm);

	printf("main ended\n");
	return 0;
}
