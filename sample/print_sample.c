#include <stdio.h>

#include "../src/em_print.h"

int main()
{
	const char *msg = "abc";
	em_printf(EM_LOG_TRACE,"EM_LOG_TRACE\n");
	em_printf(EM_LOG_DEBUG,"EM_LOG_DEBUG %s\n", msg);
	em_printf(EM_LOG_INFO,"EM_LOG_INFO %s %d\n", msg, 5);
	em_printf(EM_LOG_WARNING,"EM_LOG_WARNING\n");
	em_printf(EM_LOG_ERROR,"EM_LOG_ERROR\n");
	em_printf(EM_LOG_TOP,"EM_LOG_TOP\n");

	printf("change loglevel to EM_LOG_TRACE(%d)\n",EM_LOG_TRACE);
	em_print_set_loglevel(EM_LOG_TRACE);

	em_printf(EM_LOG_TRACE,"EM_LOG_TRACE\n");
	em_printf(EM_LOG_DEBUG,"EM_LOG_DEBUG %s\n", msg);
	em_printf(EM_LOG_INFO,"EM_LOG_INFO %s %d\n", msg, 5);
	em_printf(EM_LOG_WARNING,"EM_LOG_WARNING\n");
	em_printf(EM_LOG_ERROR,"EM_LOG_ERROR\n");
	em_printf(EM_LOG_TOP,"EM_LOG_TOP\n");

	printf("change loglevel to EM_LOG_WARNING(%d)\n",EM_LOG_WARNING);
	em_print_set_loglevel(EM_LOG_WARNING);

	em_printf(EM_LOG_TRACE,"EM_LOG_TRACE\n");
	em_printf(EM_LOG_DEBUG,"EM_LOG_DEBUG %s\n", msg);
	em_printf(EM_LOG_INFO,"EM_LOG_INFO %s %d\n", msg, 5);
	em_printf(EM_LOG_WARNING,"EM_LOG_WARNING\n");
	em_printf(EM_LOG_ERROR,"EM_LOG_ERROR\n");
	em_printf(EM_LOG_TOP,"EM_LOG_TOP\n");

	printf("change timeprint\n");
	em_print_is_timeprint(1);
	em_printf(EM_LOG_TOP,"EM_LOG_TOP with time\n");

	FILE *fp = fopen("print_sample.log", "w");
	em_print_set_outstream(fp);
	em_printf(EM_LOG_TOP,"file log 1\n");
	em_printf(EM_LOG_TOP,"file log 2\n");

	em_print_set_outstream(stdout);
	em_printf(EM_LOG_TOP,"console log\n");

	fclose(fp);

	printf("file: %s:%d\n", __FILE__, __LINE__);
	printf("func: %s\n", __FUNCTION__);

	return 0;
}
