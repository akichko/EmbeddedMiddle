/*============================================================================
MIT License

Copyright (c) 2022 akichko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/select.h>
#include "em_command.h"
#include "em_queue.h"
#include "em_print.h"

int em_cmd_init(em_cmdmng_t *cm, int num_cmd, void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	if (0 != em_gdatamng_create(&cm->dm_cmd, sizeof(em_cmdsetting_t), num_cmd,
								EM_DMNG_KEY_STRING, EM_CMD_NAME_LENGTH_MAX, EM_DMNG_DPLCT_ERROR,
								alloc_func, free_func))
	{
		em_printf(EM_LOG_ERROR, "datamng error\n");
		return -1;
	}

	if (0 != em_queue_create(&cm->qu_cmd, EM_CMD_BUF_LENGTH, EM_CMD_ASYNC_QUEUE_SIZE,
							 alloc_func, free_func))
	{
		em_printf(EM_LOG_ERROR, "queue error\n");
		return -1;
	}

	cm->is_running = 1;

	return 0;
}

int em_cmd_destroy(em_cmdmng_t *cm)
{
	em_gdatamng_destroy(&cm->dm_cmd);
	em_queue_destroy(&cm->qu_cmd);
	return 0;
}

int em_cmd_register(em_cmdmng_t *cm,
					em_cmdsetting_t *cmdsetting)
{
	if (0 != em_gdatamng_add_data(&cm->dm_cmd, cmdsetting->cmd_name, cmdsetting))
	{
		em_printf(EM_LOG_ERROR, "command register error: %s\n");
		return -1;
	}

	em_printf(EM_LOG_INFO, "command '%s' registered\n", cmdsetting->cmd_name);
	return 0;
}

static int (*_em_cmd_get_func_by_cmdname(em_cmdmng_t *cm, char *cmdname))(int, char **)
{
	em_cmdsetting_t *cmdsetting = (em_cmdsetting_t *)em_gdatamng_get_data_ptr(&cm->dm_cmd, cmdname);
	if (NULL == cmdsetting)
	{
		return NULL;
	}
	return cmdsetting->cmd_func;
}

int em_cmd_exec_args(em_cmdmng_t *cm, int argc, char **argv)
{
	if (argc == 0)
		return 0;

	int (*cmd_func)(int, char **) = _em_cmd_get_func_by_cmdname(cm, argv[0]);
	if (NULL == cmd_func)
	{
		printf("command '%s' not found\n", argv[0]);
		return -1;
	}

	return cmd_func(argc, argv);
}

#define WLEN EM_CMD_WORD_LENGTH_MAX
#define CONCAT(a, b) CONCAT2(a, b)
#define CONCAT2(a, b) %a##b // %の後にスペースがあるとNG
#define STR(s) STR2(s)
#define STR2(s) #s

#define SCANFMT(WLEN) CONCAT(WLEN, s) CONCAT(WLEN, s) CONCAT(WLEN, s) CONCAT(WLEN, s) CONCAT(WLEN, s) \
	CONCAT(WLEN, s) CONCAT(WLEN, s) CONCAT(WLEN, s) CONCAT(WLEN, s) CONCAT(WLEN, s)\n

static int _em_read_line(char *src, char **dst, int *word_num, int max_word_num, int max_word_length)
{
	if (src == NULL)
		return -1;

	int ret_scan;

	// printf("%s", STR(SCANFMT(10)));

	ret_scan = sscanf(src,
					  "%128s %128s %128s %128s %128s %128s %128s %128s %128s %128s\n",
					  // STR(SCANFMT(WLEN)),
					  dst[0], dst[1], dst[2], dst[3], dst[4],
					  dst[5], dst[6], dst[7], dst[8], dst[9]);

	if (ret_scan <= 0)
		return -1;

	for (int i = 0; i < EM_CMD_WORD_NUM_MAX; i++)
	{
	}

	*word_num = ret_scan;

	return 0;
}

static int timed_fgets(char *dst, int size, FILE *stream, int timeout_sec)
{
	if (timeout_sec >= 0)
	{
		fd_set readfds;
		int fd = 0; /* stdinのファイルディスクリプタは0 */
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		struct timeval tv = {timeout_sec, 0};

		int ret_select = select(fd + 1, &readfds, NULL, NULL, &tv);

		if (ret_select == 0) /* タイムアウトが発生 */
		{
			return -1;
		}
	}

	fgets(dst, size, stream);
	return 0;
}

int em_cmd_start(em_cmdmng_t *cm)
{
	char input_buf[EM_CMD_BUF_LENGTH];

	printf("cmd > ");
	fflush(stdout);
	cm->is_running = 1;
	while (cm->is_running)
	{
		int ret = timed_fgets(input_buf, sizeof(input_buf), stdin, 1);
		if (ret != 0)
		{
			// printf(">");
			// fflush(stdout);
			continue;
		}

		input_buf[strcspn(input_buf, "\n")] = '\0'; // 改行コードを削除する
		input_buf[EM_CMD_BUF_LENGTH - 1] = '\0';

		ret = em_cmd_exec_string(cm, input_buf);

		if (cm->is_running)
		{
			printf("cmd > ");
			fflush(stdout);
		}
	}
	return 0;
}

int em_cmd_start_bg(em_cmdmng_t *cm)
{
	char cmdstr[EM_CMD_WORD_NUM_MAX + 1][EM_CMD_WORD_LENGTH_MAX + 1];
	char *cmdstr_ptr[EM_CMD_WORD_NUM_MAX + 1];
	char input_buf[EM_CMD_BUF_LENGTH];

	for (int i = 0; i < EM_CMD_WORD_NUM_MAX; i++)
	{
		cmdstr_ptr[i] = cmdstr[i];
	}

	while (cm->is_running)
	{
		int pos_word = 0;

		// キューからコマンド取得
		int ret = em_dequeue(&cm->qu_cmd, input_buf, 100);
		if (ret != 0)
		{
			continue;
		}

		if (0 == _em_read_line(input_buf, (char **)cmdstr_ptr, &pos_word, EM_CMD_WORD_NUM_MAX, EM_CMD_WORD_LENGTH_MAX))
		{
			printf("\n[async] exec command '%s'\n", input_buf);
			em_cmd_exec_args(cm, pos_word - 1, ((char **)cmdstr_ptr) + 1);
		}
	}
	return 0;
}

int em_cmd_stop(em_cmdmng_t *cm)
{
	cm->is_running = 0;
	return 0;
}

int em_cmd_exec_string(em_cmdmng_t *cm, char *cmd_str)
{
	int ret = -1;

	char word_buf[EM_CMD_WORD_NUM_MAX + 1][EM_CMD_WORD_LENGTH_MAX + 1];
	char *word_ptr[EM_CMD_WORD_NUM_MAX + 1];
	// char input_buf[EM_CMD_WORD_NUM_MAX * (EM_CMD_WORD_LENGTH_MAX + 1)];
	int word_num;

	for (int i = 0; i < EM_CMD_WORD_NUM_MAX; i++)
	{
		word_ptr[i] = word_buf[i];
	}

	if (0 == _em_read_line(cmd_str, (char **)word_ptr, &word_num, EM_CMD_WORD_NUM_MAX, EM_CMD_WORD_LENGTH_MAX))
	{
		if (word_num > 1 && 0 == strncmp(word_ptr[0], EM_CMD_ASYNC_CMDNAME, EM_CMD_WORD_LENGTH_MAX))
		{
			if (0 != em_enqueue(&cm->qu_cmd, cmd_str, EM_NO_WAIT))
			{
				em_printf(EM_LOG_ERROR, "async cmd buffer full [%s]\n", word_ptr[1]);
			}
		}
		else
		{
			ret = em_cmd_exec_args(cm, word_num, (char **)word_ptr);
		}
	}
	return ret;
}
