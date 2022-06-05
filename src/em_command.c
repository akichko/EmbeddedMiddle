#include <stdio.h>
#include <string.h>
#include "em_command.h"

int em_cmd_init(em_cmdmng_t *cm, int num_cmd)
{
	if (0 != em_datamng_create(&cm->cmdmng, sizeof(em_cmdsetting_t), num_cmd))
	{
		printf("error\n");
		return -1;
	}
	return 0;
}

int em_cmd_register(em_cmdmng_t *cm,
					em_cmdsetting_t *cmdsetting)
{
	if (0 != em_datamng_add_data(&cm->cmdmng, cmdsetting->cmd_id, cmdsetting))
	{
		// printf("register error\n");
		return -1;
	}
	return 0;
}

static char _em_cmdname_comparator(void *dm_data, void *cmd_name)
{
	if (0 == strcmp(cmd_name, ((em_cmdsetting_t *)dm_data)->cmd_name))
	{
		return 1;
	}
	return 0;
}

static void *_em_cmd_get_func_by_cmdname(em_cmdmng_t *cm, char *cmdname)
{
	em_cmdsetting_t cmdsetting;
	if (0 != em_datamng_get_data_by_func(&cm->cmdmng, cmdname, _em_cmdname_comparator, &cmdsetting))
	{
		return NULL;
	}
	return cmdsetting.cmd_func;
}

static int _em_read_word(char *dst, int max_size)
{
	int ret = -1;
	char c;
	int pos_letter = 0;

	while (1) // 1単語
	{
		c = getchar();

		if (c == ' ')
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

int em_cmd_start(em_cmdmng_t *cm)
{
	char cmdstr[EM_CMD_WORD_NUM_MAX + 1][EM_CMD_WORD_LENGTH_MAX + 1];
	char *cmdstr_ptr[EM_CMD_WORD_NUM_MAX + 1];
	for (int i = 0; i < EM_CMD_WORD_NUM_MAX; i++)
	{
		cmdstr_ptr[i] = cmdstr[i];
	}

	char c;
	while (1) // 1行
	{
		memset(cmdstr, 0, sizeof(cmdstr));
		int pos_word = 0;
		int pos_letter = 0;
		printf("cmd > ");
		int ret;
		int exec;

		while (1) // 1単語
		{
			exec = 1;
			ret = _em_read_word(cmdstr[pos_word], EM_CMD_WORD_LENGTH_MAX);

			if (ret < 0)
			{
				printf("word length over %d\n", EM_CMD_WORD_LENGTH_MAX);
				_em_read_until_line_end();
				exec = 0;

				break;
			}
			else if (ret == 0)
			{
				if (pos_word >= EM_CMD_WORD_NUM_MAX)
				{
					printf("word num over %d\n", EM_CMD_WORD_NUM_MAX);
					_em_read_until_line_end();
					exec = 0;
					break;
				}
				pos_word++;
				continue;
			}
			else if (ret == 1) //最終単語
			{
				if (pos_word >= EM_CMD_WORD_NUM_MAX)
				{
					printf("word num over %d\n", EM_CMD_WORD_NUM_MAX);
					if (pos_word > EM_CMD_WORD_NUM_MAX)
					{
						_em_read_until_line_end();
					}
					exec = 0;
					break;
				}
				pos_word++;
				break;
			}
			else if (ret == 2) //スペース＋改行
			{
				if (pos_word == 0)
				{
					exec = 0;
				}
				break;
			}
			else
			{
				printf("em_cmd_start unknown error\n");
				return -1;
			}
		}

		if (exec)
		{
			em_cmd_exec(cm, pos_word, (char **)cmdstr_ptr);
		}
	}
}

int em_cmd_exec(em_cmdmng_t *cm, int argc, char **argv)
{
#if 0
	printf("[%d] ", argc);
	for (int i = 0; i < argc; i++)
	{
		printf("%s ", argv[i]);
	}
	printf("\n");
#endif
	void (*cmd_func)(int, char **);
	cmd_func = _em_cmd_get_func_by_cmdname(cm, argv[0]);
	if (NULL == cmd_func)
	{
		printf("command '%s' not found\n", argv[0]);
	}
	else
	{
		cmd_func(argc, argv);
	}
}
