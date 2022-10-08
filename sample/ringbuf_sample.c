#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../src/em_ringbuf.h"

int add_buf(em_ring_t *rb, int val)
{
	int *setbuf;
	printf("add %d\n", val);
	setbuf = em_ring_get_dataptr_new(rb);
	if (setbuf == NULL)
		return -1;
	*setbuf = val;
	return em_ring_add_newdata(rb);
}

int main()
{
	em_ring_t rb;
	int *setbuf;
	int *getbuf;
	int ret;

	em_ring_create(&rb, sizeof(int), 4, EM_RINGBUF_ERROR, &malloc, &free);
	em_ring_print(&rb);

	if (0 != add_buf(&rb, 10))
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	getbuf = em_ring_get_dataptr_head(&rb, 0);
	em_printf(EM_LOG_INFO, "getbuf(head 0) = %d\n", *getbuf);

	if(NULL == em_ring_get_dataptr_head(&rb, 1))
		em_printf(EM_LOG_INFO, "getbuf(head 1) = NULL\n");

	if (0 != add_buf(&rb, 20))
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	getbuf = em_ring_get_dataptr_head(&rb, 0);
	em_printf(EM_LOG_INFO, "getbuf(head 0) = %d\n", *getbuf);

	getbuf = em_ring_get_dataptr_head(&rb, 1);
	em_printf(EM_LOG_INFO, "getbuf(head 1) = %d\n", *getbuf);

	getbuf = em_ring_get_dataptr_tail(&rb, 0);
	em_printf(EM_LOG_INFO, "getbuf(tail 0) = %d\n", *getbuf);

	if (0 != add_buf(&rb, 30))
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	if (0 != add_buf(&rb, 40))
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	printf("add\n");
	if (NULL == em_ring_get_dataptr_new(&rb)) // error
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	printf("del num=1\n");
	if (0 != em_ring_delete_taildata(&rb, 1))
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	printf("del num=2\n");
	if (0 != em_ring_delete_taildata(&rb, 2))
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	if (0 != add_buf(&rb, 60))
		em_printf(EM_LOG_ERROR, "error\n");
	em_ring_print(&rb);

	getbuf = em_ring_get_dataptr_head(&rb, 0);
	em_printf(EM_LOG_INFO, "getbuf(head 0) = %d\n", *getbuf);

	getbuf = em_ring_get_dataptr_head(&rb, 1);
	em_printf(EM_LOG_INFO, "getbuf(head 1) = %d\n", *getbuf);

	getbuf = em_ring_get_dataptr_tail(&rb, 0);
	em_printf(EM_LOG_INFO, "getbuf(tail 0) = %d\n", *getbuf);

	getbuf = em_ring_get_dataptr_tail(&rb, 1);
	em_printf(EM_LOG_INFO, "getbuf(tail 1) = %d\n", *getbuf);

	em_ring_destroy(&rb);
}