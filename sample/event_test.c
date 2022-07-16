#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>
#include "../src/em_eventflag.h"


static void *thread1(void *);

em_event_t event;

int main(void)
{
	pthread_t thread_id = 0;


	em_event_init(&event);

	if (pthread_create(&thread_id, NULL, thread1, NULL) < 0)
	{
		printf("ERROR for creating thread ");
		exit(1);
	}
	printf("[Main] Thread created, wait event set\n");

	em_event_wait(&event, EM_NO_TIMEOUT);

	printf("[Main] Main is working\n");

	if (pthread_join(thread_id, NULL) < 0)
	{
		printf("ERROR for finishing thread \n");
		exit(1);
	}

	printf("[Main] Main End\n");
}

void *thread1(void *arg)
{
	printf("[Sub] Thread is working\n");
	sleep(2);

	printf("[Sub] set event\n");
	em_event_set(&event);
}