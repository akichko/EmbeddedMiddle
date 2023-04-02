#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "em_ether.h"

#define SERVER_PORT 10100
#define CLIENT_PORT 10200
#define MESSAGE_SIZE 256

// シグナルハンドラ
void sig_handler(int sig)
{
	if (sig == SIGPIPE)
	{
		// 何らかのエラー処理
		;
	}
	// 何もしない
	return;
}

void *server_thread(void *arg)
{
	int count = 0;

	// SIGPIPEシグナルを無視する
	// signal(SIGPIPE, sig_handler);

	// スレッドがシグナルをブロックする
	sigset_t sig_mask;
	sigemptyset(&sig_mask);
	sigaddset(&sig_mask, SIGPIPE);
	pthread_sigmask(SIG_BLOCK, &sig_mask, NULL);

	while (1)
	{
		em_socket_t sk_listen;
		em_socket_t sk_client;
		if (0 != em_tcp_connect_server(&sk_listen, &sk_client, "0.0.0.0", SERVER_PORT, FALSE, EM_NO_TIMEOUT))
		{
			em_printf(EM_LOG_ERROR, "Server: connect error\n");
			continue;
		}
		em_printf(EM_LOG_INFO, "Server: connected\n");

		while (1)
		{
			count++;
			unsigned char message[1024];
			sprintf(message, "count=%d", count);
			int data_size = strlen(message);

			if (0 != em_tcp_send(&sk_client, message, data_size, EM_NO_TIMEOUT))
			{
				em_printf(EM_LOG_ERROR, "Server: send error\n");
				break;
			}
			sleep(1);
		}
		em_tcp_disconnect(&sk_client);
		em_socket_close(&sk_listen);
	}
	return NULL;
}

void *client_thread(void *arg)
{
	int count = 0;

	while (1)
	{
		em_socket_t sk;
		if (0 != em_tcp_connect_client(&sk, "0.0.0.0", CLIENT_PORT, "127.0.0.1", SERVER_PORT, TRUE, EM_NO_TIMEOUT))
		{
			em_printf(EM_LOG_ERROR, "Client: connect error\n");
			sleep(1);
			continue;
		}
		em_printf(EM_LOG_INFO, "Client: connected\n");
		sleep(1);

		while (1)
		{
			unsigned char recv_buf[MESSAGE_SIZE];
			memset(recv_buf, 0, MESSAGE_SIZE);

			int len;
			if (0 > (len = em_tcp_recv(&sk, recv_buf, MESSAGE_SIZE, EM_NO_TIMEOUT)))
			{
				continue;
			}
			em_printf(EM_LOG_INFO, "Client received (len=%d): %s\n", len, recv_buf);

			count++;
			if (count % 10 == 0)
			{
				break;
			}
		}

		em_tcp_disconnect(&sk);
		em_printf(EM_LOG_INFO, "Client: disconnected\n");
		sleep(3);
	}
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
