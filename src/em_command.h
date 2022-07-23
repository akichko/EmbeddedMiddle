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
	const char *cmd_name;
	void (*cmd_func)(int, char **);
} em_cmdsetting_t;

typedef struct
{
	char is_running;
	em_datamng_t cmdmng;
} em_cmdmng_t;

int em_cmd_init(em_cmdmng_t *cm,
				int num_cmd);

int em_cmd_start(em_cmdmng_t *cm);

int em_cmd_stop(em_cmdmng_t *cm);

int em_cmd_regist(em_cmdmng_t *cm,
				  em_cmdsetting_t *cmdsetting);

int em_cmd_exec(em_cmdmng_t *cm,
				int argc,
				char **argv);

#endif // EM_COMMAND_H
