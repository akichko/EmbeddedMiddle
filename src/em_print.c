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
}
int em_print_is_timeprint(int is_timeprint)
{
	s_is_timeprint = is_timeprint;
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
	char *tmppos;
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
