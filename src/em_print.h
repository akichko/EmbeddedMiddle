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
#ifndef __EM_PRINT_H__
#define __EM_PRINT_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#define EM_LOG_TOP 6
#define EM_LOG_ERROR 5
#define EM_LOG_WARNING 4
#define EM_LOG_INFO 3
#define EM_LOG_DEBUG 2
#define EM_LOG_TRACE 1
#define EM_LOG_DEFAULT EM_LOG_INFO

	typedef struct tag_printsetting
{
	int maxlength;
	int loglevel;
	int is_timeprint;
	int is_errorlevelprint;
	int is_funcprint;
	int is_stdout;
	FILE *outstream;
} em_printsetting_t;

#define em_printf(type, fmt, ...) _em_printf(NULL, __FILE__, __FUNCTION__, __LINE__, type, fmt, ##__VA_ARGS__)

#define em_printfext(setting, type, fmt, ...) _em_printf(setting, __FILE__, __FUNCTION__, __LINE__, type, fmt, ##__VA_ARGS__)

int _em_printf(em_printsetting_t *setting,
				const char *file,
				const char *function,
				int line,
				int type,
				const char *fmt,
				...);

int em_print_set_loglevel(int newlevel);
int em_print_is_timeprint(int is_timeprint);
int em_print_set_outstream(FILE *outstream);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__EM_PRINT_H__
