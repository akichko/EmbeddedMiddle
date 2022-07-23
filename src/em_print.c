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
#include <sys/types.h>
#include <unistd.h>
#include "em_print.h"

static int s_loglevel = EM_LOG_DEFAULT;
static int s_is_timeprint = 0;

int em_print_set_loglevel(int newlevel)
{
	s_loglevel = newlevel;
	return 0;
}

int em_print_is_timeprint(int is_timeprint)
{
	s_is_timeprint = is_timeprint;
	return 0;
}

void _em_printf(const char *file, const char *function, int line, int type, const char *fmt, ...)
{
	if (type < s_loglevel)
	{
		return;
	}

	if (s_is_timeprint)
	{
		time_t timer;
		time(&timer);
		char *t = ctime(&timer);
		t[strlen(t) - 1] = '\0';
		fprintf(stdout, "[%s] ", t);
	}

	const char *filename = file;
	const char *tmppos;
	while (NULL != (tmppos = strstr(filename, "/")))
	{
		filename = tmppos + 1;
	}
	fprintf(stdout, "[%d][%s:%d %s] ", type, filename, line, function);

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
}
