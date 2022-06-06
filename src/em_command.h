#ifndef EM_COMMAND_H
#define EM_COMMAND_H

#include "em_datamng.h"

#define EM_LAST_WORD 1
#define EM_LAST_EMPTY 2
#define EM_WORD_SUCCESS 0
#define EM_WORD_ERROR -1

#define EM_CMD_WORD_NUM_MAX 10
#define EM_CMD_WORD_LENGTH_MAX 16

typedef struct
{
	int cmd_id;
	char *cmd_name;
	void (*cmd_func)(int, char **);
} em_cmdsetting_t;

typedef struct
{
	em_datamng_t cmdmng;
} em_cmdmng_t;

int em_cmd_init(em_cmdmng_t *cm,
				int num_cmd);

int em_cmd_start(em_cmdmng_t *cm);

int em_cmd_regist(em_cmdmng_t *cm,
				  em_cmdsetting_t *cmdsetting);

int em_cmd_exec(em_cmdmng_t *cm,
				int argc,
				char **argv);

#endif // EM_COMMAND_H
