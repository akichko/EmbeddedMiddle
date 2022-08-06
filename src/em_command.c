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
#include "em_command.h"
#include "em_print.h"

#define EM_LAST_WORD 1
#define EM_LAST_EMPTY 2
#define EM_WORD_SUCCESS 0
#define EM_WORD_ERROR -1

int em_cmd_init(em_cmdmng_t *cm, int num_cmd, void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	if (0 != em_gdatamng_create(&cm->cmdmng, sizeof(em_cmdsetting_t), num_cmd,
								EM_DMNG_KEY_STRING, EM_CMD_NAME_LENGTH_MAX, EM_DMNG_DPLCT_ERROR,
								alloc_func, free_func))
	{
		em_printf(EM_LOG_ERROR, "error\n");
		return -1;
	}
	return 0;
}

int em_cmd_destroy(em_cmdmng_t *cm)
{
	return em_gdatamng_delete(&cm->cmdmng);
}

int em_cmd_regist(em_cmdmng_t *cm,
				  em_cmdsetting_t *cmdsetting)
{
	if (0 != em_gdatamng_add_data(&cm->cmdmng, cmdsetting->cmd_name, cmdsetting))
	{
		em_printf(EM_LOG_ERROR, "cmd register error\n");
		return -1;
	}
	return 0;
}

static void (*_em_cmd_get_func_by_cmdname(em_cmdmng_t *cm, char *cmdname))(int, char **)
{
	em_cmdsetting_t *cmdsetting = (em_cmdsetting_t *)em_gdatamng_get_data_ptr(&cm->cmdmng, cmdname);
	if (NULL == cmdsetting)
	{
		return NULL;
	}
	return cmdsetting->cmd_func;
}

static int _em_read_until_line_end()
{
	char c;

	while (1) // 1単語
	{
		c = getchar();

		if (c == '\n')
		{
			return 0;
		}
	}
}

static int _em_read_word(char *dst, int max_size)
{
	char c;
	int pos_letter = 0;

	while (1) // 1単語
	{
		c = getchar();

		if (c == ' ' || c == '\t')
		{
			if (pos_letter > 0)
			{
				dst[pos_letter] = '\0';
				return 0;
			}
		}
		else if (c == EOF || c == '\n')
		{
			dst[pos_letter] = '\0';
			if (pos_letter == 0)
			{
				return 2; //空文字
			}
			return 1; //最終単語
		}
		else //正常文字
		{
			if (pos_letter >= max_size - 1)
			{
				dst[pos_letter] = '\0';
				return -1; //文字数オーバー
			}
			dst[pos_letter] = c;
			pos_letter++;
		}
	}
}

static int _em_read_line(char **dst, int *word_num, int max_word_num, int max_word_length)
{
	int pos_word = 0;
	int ret_word;
	int ret = -1;

	while (1) // 1単語
	{
		ret_word = _em_read_word(dst[pos_word], EM_CMD_WORD_LENGTH_MAX);

		if (ret_word < 0)
		{
			em_printf(EM_LOG_ERROR, "word length over %d\n", EM_CMD_WORD_LENGTH_MAX);
			_em_read_until_line_end();
			break;
		}
		else if (ret_word == 0)
		{
			if (pos_word >= EM_CMD_WORD_NUM_MAX)
			{
				em_printf(EM_LOG_ERROR, "word num over %d\n", EM_CMD_WORD_NUM_MAX);
				_em_read_until_line_end();
				break;
			}
			pos_word++;
			continue;
		}
		else if (ret_word == 1) //最終単語＋改行
		{
			if (pos_word >= EM_CMD_WORD_NUM_MAX)
			{
				em_printf(EM_LOG_ERROR, "word num over %d\n", EM_CMD_WORD_NUM_MAX);
				if (pos_word > EM_CMD_WORD_NUM_MAX)
				{
					_em_read_until_line_end();
				}
				break;
			}
			pos_word++;
			ret = 0;
			break;
		}
		else if (ret_word == 2) //空単語＋改行
		{
			if (pos_word > 0)
			{
				ret = 0;
			}
			break;
		}
		else
		{
			em_printf(EM_LOG_ERROR, "_em_read_line unknown error\n");
			break;
		}
	}
	*word_num = pos_word;

	return ret;
}

int em_cmd_start(em_cmdmng_t *cm)
{
	char cmdstr[EM_CMD_WORD_NUM_MAX + 1][EM_CMD_WORD_LENGTH_MAX + 1];
	char *cmdstr_ptr[EM_CMD_WORD_NUM_MAX + 1];
	for (int i = 0; i < EM_CMD_WORD_NUM_MAX; i++)
	{
		cmdstr_ptr[i] = cmdstr[i];
	}

	cm->is_running = 1;
	while (cm->is_running)
	{
		memset(cmdstr, 0, sizeof(cmdstr));
		int pos_word = 0;
		printf("cmd > ");

		if (0 == _em_read_line((char **)cmdstr_ptr, &pos_word, EM_CMD_WORD_NUM_MAX, EM_CMD_WORD_LENGTH_MAX))
		{
			em_cmd_exec(cm, pos_word, (char **)cmdstr_ptr);
		}
	}
	return 0;
}

int em_cmd_stop(em_cmdmng_t *cm)
{
	cm->is_running = 0;
	return 0;
}

int em_cmd_exec(em_cmdmng_t *cm, int argc, char **argv)
{
	void (*cmd_func)(int, char **) = _em_cmd_get_func_by_cmdname(cm, argv[0]);
	if (NULL == cmd_func)
	{
		printf("command '%s' not found\n", argv[0]);
		return -1;
	}

	cmd_func(argc, argv);
	return 0;
}
