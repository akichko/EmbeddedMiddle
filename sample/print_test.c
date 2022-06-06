#include <stdio.h>

#include "../src/em_print.h"

int main()
{
	char *msg = "bbb";
	EM_PRINTF(1,"aaa\n");
	EM_PRINTF(2,"aaa %s\n", msg);
	EM_PRINTF(3,"aaa %s %d\n", msg, 5);

	printf("file: %s:%d\n", __FILE__, __LINE__);
	printf("func: %s\n", __FUNCTION__);

	return 0;
}
