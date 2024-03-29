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

#include "em_gdatamng.h"
#include "em_queue.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define EM_CMD_NAME_LENGTH_MAX 16
#define EM_CMD_WORD_NUM_MAX 10
#define EM_CMD_WORD_LENGTH_MAX 128
#define EM_CMD_BUF_LENGTH (EM_CMD_WORD_NUM_MAX * (EM_CMD_WORD_LENGTH_MAX + 1))
#define EM_CMD_ASYNC_CMDNAME "async"
#define EM_CMD_ASYNC_QUEUE_SIZE 4

typedef struct tag_cmdsetting
{
	const char *cmd_name;
	int (*cmd_func)(int, char **);
} em_cmdsetting_t;

typedef struct
{
	char is_running;
	em_datamng_t dm_cmd;
	em_queue_t qu_cmd;
} em_cmdmng_t;

int em_cmd_init(em_cmdmng_t *cm,
				int num_cmd,
				void *(*alloc_func)(size_t),
				void (*free_func)(void *));

int em_cmd_destroy(em_cmdmng_t *cm);

int em_cmd_start(em_cmdmng_t *cm);

int em_cmd_start_bg(em_cmdmng_t *cm);

int em_cmd_stop(em_cmdmng_t *cm);

int em_cmd_register(em_cmdmng_t *cm,
					em_cmdsetting_t *cmdsetting);

int em_cmd_exec_string(em_cmdmng_t *cm,
					   char *cmd_str);

int em_cmd_exec_args(em_cmdmng_t *cm,
					 int argc,
					 char **argv);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // EM_COMMAND_H
