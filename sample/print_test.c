#include <stdio.h>

#include "../src/em_print.h"

int main()
{
	char *msg = "bbb";
	em_printf(1,"aaa\n");
	em_printf(2,"aaa %s\n", msg);
	em_printf(3,"aaa %s %d\n", msg, 5);

	printf("file: %s:%d\n", __FILE__, __LINE__);
	printf("func: %s\n", __FUNCTION__);

	return 0;
}
