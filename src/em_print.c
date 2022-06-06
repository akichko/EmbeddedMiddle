#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include "em_print.h"


void em_printf(const char *file, const char *function, int line, int type, const char *fmt, ...)
{
	time_t timer;
	time(&timer);
	char *t = ctime(&timer);
	t[strlen(t)-1] = '\0';

	const char *filename = file;
	char *tmppos;
	while(NULL != (tmppos = strstr(filename, "/"))){
		filename = tmppos + 1;
	}
	fprintf(stdout, "[%s] [%d] [%s:%d %s] ", t, type, filename, line, function);

	va_list ap;
	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	va_end(ap);
}
