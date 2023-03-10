#include <stdio.h>

#include "../src/em_print.h"

int main()
{
	int ret = 0;
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


	printf("file: %s:%d\n", __FILE__, __LINE__);
	printf("func: %s\n", __FUNCTION__);

	em_printsetting_t setting = {512, EM_LOG_INFO, 0, 0, 0, 1, NULL};
	ret = em_printfext(&setting, EM_LOG_INFO, "no print test\n");
	setting.maxlength= 5;
	ret = em_printfext(&setting, EM_LOG_INFO, "1234567890\n");
	setting.maxlength= 512;
	em_printfext(&setting, EM_LOG_INFO, "---\n");
	setting.is_timeprint = 1;
	em_printfext(&setting, EM_LOG_INFO, "time print test\n");
	setting.is_errorlevelprint = 1;
	em_printfext(&setting, EM_LOG_INFO, "time & errorlevel print test\n");
	setting.is_funcprint = 1;
	em_printfext(&setting, EM_LOG_INFO, "time & errorlevel & func print test\n");
	setting.outstream = fp;
	em_printfext(&setting, EM_LOG_INFO, "stdout & file output test\n");

	fclose(fp);

	return ret;
}
