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
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "em_print.h"

static char s_loglevel_char[] = {' ', 'T', 'D', 'I', 'W', 'E', '-'};

static em_printsetting_t defaultsetting = {512, EM_LOG_DEFAULT, 0, 1, 1, 1, NULL};

int em_print_set_loglevel(int newlevel)
{
	defaultsetting.loglevel = newlevel;
	return 0;
}

int em_print_is_timeprint(int is_timeprint)
{
	defaultsetting.is_timeprint = is_timeprint;
	return 0;
}

int em_print_set_outstream(FILE *outstream)
{
	defaultsetting.outstream = outstream;
	return 0;
}

int _em_printf(em_printsetting_t *setting, const char *file, const char *function, int line, int type, const char *fmt, ...)
{
	em_printsetting_t *psetting = &defaultsetting;
	if (setting != NULL)
		psetting = setting;

	char buf[psetting->maxlength];
	int length;
	int total_length = 0;

	if (type < psetting->loglevel)
	{
		return 0;
	}

	if (psetting->is_timeprint)
	{
		struct timeval tv;
		struct tm *time_st;
		gettimeofday(&tv, NULL);
		time_st = localtime(&tv.tv_sec);

		length = snprintf(buf + total_length, psetting->maxlength - total_length, "[%d/%02d/%02d %02d:%02d:%02d.%03ld] ", // 現在時刻
						  time_st->tm_year + 1900,
						  time_st->tm_mon + 1,
						  time_st->tm_mday,
						  time_st->tm_hour,
						  time_st->tm_min,
						  time_st->tm_sec,
						  tv.tv_usec / 1000);
		total_length += length;
	}

	if (psetting->is_errorlevelprint)
	{
		length = snprintf(buf + total_length, psetting->maxlength - total_length, "[%c] ", s_loglevel_char[type]);
		total_length += length;
	}

	if (psetting->is_funcprint)
	{
		const char *filename = file;
		const char *tmppos;
		while (NULL != (tmppos = strstr(filename, "/")))
		{
			filename = tmppos + 1;
		}
		length = snprintf(buf + total_length, psetting->maxlength - total_length, "[%s:%d %s] ", filename, line, function);
		total_length += length;
	}

	va_list ap;
	va_start(ap, fmt);
	if (0 < psetting->maxlength - total_length)
	{
		length = vsnprintf(buf + total_length, psetting->maxlength - total_length, fmt, ap);
		total_length += length;
	}
	va_end(ap);

	if (psetting->is_stdout == 1)
		fprintf(stdout, "%s", buf);

	if (psetting->outstream != NULL)
		fprintf(psetting->outstream, "%s", buf);

	return strlen(buf);
}
