#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "em_ether.h"

#define SERVER_PORT 10100
#define CLIENT_PORT 10200
#define MESSAGE_SIZE 256

void *server_thread(void *arg)
{
	em_socket_t sk;
	em_socket_t sk_client;
	if (0 != em_tcp_connect_server(&sk, &sk_client, "127.0.0.1", SERVER_PORT, FALSE, EM_NO_TIMEOUT))
	{
		printf("Server: connect error\n");
	}
	printf("Server: connected\n");

	while (1)
	{
		unsigned char recv_buf[1024];
		memset(recv_buf, 0, sizeof(recv_buf));
		int len;

		if (0 > (len = em_tcp_recv(&sk_client, recv_buf, MESSAGE_SIZE, EM_NO_TIMEOUT)))
		{
			continue;
		}
		printf("Server received (%d): %s\n", len, recv_buf);

		usleep(100000); // 100ms wait
	}

	em_socket_close(&sk);
	return NULL;
}

void *client_thread(void *arg)
{
	em_socket_t sk;
	if (0 != em_tcp_connect_client(&sk, "0.0.0.0", CLIENT_PORT, "127.0.0.1", SERVER_PORT, TRUE, EM_NO_TIMEOUT))
	{
		printf("Client: connect error\n");
		return NULL;
	}
	printf("Client: connected\n");
	sleep(1);

	while (1)
	{
		unsigned char message[MESSAGE_SIZE];
		printf("Enter message: ");
		fgets((char*)message, MESSAGE_SIZE, stdin);

		int data_size = strlen((char*)message);

		if (0 != em_tcp_send(&sk, message, data_size, EM_NO_TIMEOUT))
		{
			printf("Client: send error\n");
		}

		usleep(100000); // 100ms wait
	}

	em_socket_close(&sk);
	return NULL;
}

int main()
{
	pthread_t server, client;

	pthread_create(&server, NULL, server_thread, NULL);
	sleep(1);
	pthread_create(&client, NULL, client_thread, NULL);

	pthread_join(server, NULL);
	pthread_join(client, NULL);

	return 0;
}
