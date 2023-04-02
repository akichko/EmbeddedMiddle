/*============================================================================
MIT License

Copyright (c) 2022 akichko

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
============================================================================*/
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "em_ether.h"
#include "em_queue.h"
#include "em_time.h"
#include "em_print.h"

// socket

int em_socket_init(em_socket_t *sk, em_ethertype_t type,
				   const char *local_ip, const uint16_t local_port, char is_reuseaddr)
{
	sk->is_connected = FALSE;

	switch (type)
	{
	case EM_ETHER_TYPE_UDP:
		sk->sock = socket(AF_INET, SOCK_DGRAM, 0);
		break;
	case EM_ETHER_TYPE_TCP:
		sk->sock = socket(AF_INET, SOCK_STREAM, 0);
		break;
	default:
		em_printf(EM_LOG_ERROR, "type error\n");
		return EM_ERR_PARAM;
	}

	if (sk->sock == -1)
	{
		em_printf(EM_LOG_ERROR, "socket error\n");
		return -1;
	}

	if (local_ip != NULL)
	{
		sk->local_addr.sin_family = AF_INET;
		sk->local_addr.sin_port = htons(local_port);
		sk->local_addr.sin_addr.s_addr = inet_addr(local_ip);

		if (is_reuseaddr)
		{
			// SO_REUSEADDRオプションの設定
			int optval = 1;
			setsockopt(sk->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
		}

		if (0 != bind(sk->sock, (struct sockaddr *)&sk->local_addr, sizeof(sk->local_addr)))
		{
			em_printf(EM_LOG_ERROR, "bind error\n");
			close(sk->sock);
			return -1;
		}
	}

	return 0;
}

void em_socket_close(em_socket_t *sk)
{
	if (sk == NULL)
	{
		return;
	}
	close(sk->sock);
}

int em_socket_set_destination(em_socket_t *sk, const char *dest_ip, const uint16_t dest_port)
{
	if (dest_ip == NULL || strlen(dest_ip) == 0 || dest_port == 0)
	{
		em_printf(EM_LOG_ERROR, "Invalid destination address or port\n");
		return EM_ERR_PARAM;
	}
	sk->remote_addr.sin_family = AF_INET;
	sk->remote_addr.sin_port = htons(dest_port);
	sk->remote_addr.sin_addr.s_addr = inet_addr(dest_ip);

	return 0;
}

// UDP

int em_udp_tx_init(em_socket_t *sk, const char *local_ip, const uint16_t local_port,
				   const char *dest_ip, const uint16_t dest_port)
{
	if (0 != em_socket_init(sk, EM_ETHER_TYPE_UDP, local_ip, local_port, FALSE))
	{
		return -1;
	}
	if (0 != em_socket_set_destination(sk, dest_ip, dest_port))
	{
		return -2;
	}

	return 0;
}

int em_udp_rx_init(em_socket_t *sk, const char *local_ip, const uint16_t local_port)
{
	if (0 != em_socket_init(sk, EM_ETHER_TYPE_UDP, local_ip, local_port, FALSE))
	{
		return -1;
	}

	return 0;
}

int em_udp_send(em_socket_t *sk, em_ethpacket_t *packet, int timeout_ms)
{
	if (timeout_ms != EM_NO_TIMEOUT)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(sk->sock, &fds);

		struct timeval tv = em_calc_timeval(timeout_ms);

		if (0 == select(sk->sock + 1, NULL, &fds, NULL, &tv))
		{
			em_printf(EM_LOG_DEBUG, "em_udp_send timeout [%ds]\n", timeout_ms);
			return EM_ERR_TIMEOUT;
		}
	}

	sendto(sk->sock, packet->data, packet->length, 0,
		   (struct sockaddr *)&sk->remote_addr, sizeof(sk->remote_addr));

	return 0;
}

int em_udp_send2(em_socket_t *sk, em_ethpacket_t *packet,
				 const char *dest_ip, const uint16_t dest_port, int timeout_ms)
{
	struct sockaddr_in dst_addr = sk->remote_addr;

	if (dest_ip == NULL || dest_port == 0)
	{
		em_printf(EM_LOG_ERROR, "em_udp_send param error\n");
		return EM_ERR_PARAM;
	}

	dst_addr.sin_family = AF_INET;
	dst_addr.sin_port = htons(dest_port);
	dst_addr.sin_addr.s_addr = inet_addr(dest_ip);

	if (timeout_ms != EM_NO_TIMEOUT)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(sk->sock, &fds);

		struct timeval tv = em_calc_timeval(timeout_ms);

		if (0 == select(sk->sock + 1, NULL, &fds, NULL, &tv))
		{
			em_printf(EM_LOG_DEBUG, "em_udp_send timeout [%ds]\n", timeout_ms);
			return EM_ERR_TIMEOUT;
		}
	}

	sendto(sk->sock, packet->data, packet->length, 0,
		   (struct sockaddr *)&dst_addr, sizeof(dst_addr));

	return 0;
}

int em_udp_recv(em_socket_t *sk, em_ethpacket_t *packet, int timeout_ms)
{
	if (timeout_ms != EM_NO_TIMEOUT)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(sk->sock, &fds);

		struct timeval tv = em_calc_timeval(timeout_ms);

		if (0 == select(sk->sock + 1, &fds, NULL, NULL, &tv))
		{
			em_printf(EM_LOG_DEBUG, "em_udp_recv timeout [%ds]\n", timeout_ms);
			return EM_ERR_TIMEOUT;
		}
	}

	memset(packet, 0, sizeof(*packet));
	socklen_t src_addr_len = sizeof(packet->src_addr);

	int ret = recvfrom(sk->sock, packet->data, sizeof(packet->data), 0, (struct sockaddr *)&packet->src_addr, &src_addr_len);
	if (ret < 0)
	{
		return -1;
	}
	packet->length = ret;
	// printf("received from %s:%d\n", inet_ntoa(packet->src_addr.sin_addr), ntohs(packet->src_addr.sin_port));

	return 0;
}

// TCP

int em_tcp_connect_client(em_socket_t *sk,
						  const char *local_ip, const uint16_t local_port,
						  const char *dest_ip, const uint16_t dest_port,
						  char is_reuseaddr, int timeout_ms)
{
	int ret;
	if (0 != (ret = em_socket_set_destination(sk, dest_ip, dest_port)))
	{
		return ret;
	}

	if (0 != (ret = em_socket_init(sk, EM_ETHER_TYPE_TCP, local_ip, local_port, is_reuseaddr)))
	{
		return ret;
	}

	if (0 > connect(sk->sock, (struct sockaddr *)&sk->remote_addr, sizeof(sk->remote_addr)))
	{
		em_printf(EM_LOG_ERROR, "connect error\n");
		close(sk->sock);
		return -1;
	}

	sk->is_connected = TRUE;

	return 0;
}

int em_tcp_connect_server(em_socket_t *sk_listen, em_socket_t *sk_client,
						  const char *local_ip, const uint16_t local_port,
						  char is_reuseaddr, int timeout_ms)
{
	if (0 != em_socket_init(sk_listen, EM_ETHER_TYPE_TCP, local_ip, local_port, is_reuseaddr))
	{
		em_printf(EM_LOG_ERROR, "Server: socket init error\n");
		return -1;
	}

	em_printf(EM_LOG_INFO, "listen start on %s:%d\n", local_ip, local_port);

	if (listen(sk_listen->sock, 1) < 0)
	{
		em_printf(EM_LOG_ERROR, "listen error\n");
		close(sk_listen->sock);
		return -1;
	}

	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	memset(&client_addr, 0, client_addr_len);

	int client_sock = accept(sk_listen->sock, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_sock < 0)
	{
		em_printf(EM_LOG_ERROR, "accept error\n");
		close(sk_listen->sock);
		return -1;
	}

	char client_ip[32];
	inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, sizeof(client_ip));
	ushort client_port = ntohs(client_addr.sin_port);
	em_printf(EM_LOG_INFO, "connection accepted on %s:%d\n", client_ip, client_port);

	sk_client->sock = client_sock;
	sk_client->is_connected = TRUE;
	memcpy(&sk_client->local_addr, &sk_listen->local_addr, sizeof(sk_listen->local_addr));
	memcpy(&sk_client->remote_addr, &client_addr, sizeof(client_addr));

	return 0;
}

int em_tcp_disconnect(em_socket_t *sk)
{
	if (sk == NULL)
	{
		return -1;
	}

	// ソケットが接続中の場合、切断を試みる
	shutdown(sk->sock, SHUT_RDWR);

	sk->is_connected = FALSE;
	em_socket_close(sk);
	return 0;
}

int em_tcp_send(em_socket_t *sk, unsigned char *send_buf, int size, int timeout_ms)
{
	if (sk->is_connected == FALSE)
	{
		return -1;
	}

	if (timeout_ms != EM_NO_TIMEOUT)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(sk->sock, &fds);

		struct timeval tv = em_calc_timeval(timeout_ms);

		if (0 == select(sk->sock + 1, NULL, &fds, NULL, &tv))
		{
			em_printf(EM_LOG_DEBUG, "em_tcp_send timeout [%ds]\n", timeout_ms);
			return EM_ERR_TIMEOUT;
		}
	}

	int len = send(sk->sock, send_buf, size, 0);
	if (len < 0)
	{
		em_printf(EM_LOG_ERROR, "send error\n");
		return -1;
	}
	return 0;
}

int em_tcp_recv(em_socket_t *sk, unsigned char *recv_buf, int size, int timeout_ms)
{
	if (sk->is_connected == FALSE)
	{
		return -1;
	}

	if (timeout_ms != EM_NO_TIMEOUT)
	{
		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(sk->sock, &fds);

		struct timeval tv = em_calc_timeval(timeout_ms);

		if (0 == select(sk->sock + 1, &fds, NULL, NULL, &tv))
		{
			em_printf(EM_LOG_DEBUG, "em_tcp_recv timeout [%ds]\n", timeout_ms);
			return EM_ERR_TIMEOUT;
		}
	}

	int len = recv(sk->sock, recv_buf, size, 0);
	if (len < 0)
	{
		em_printf(EM_LOG_ERROR, "recv error\n");
		return -1;
	}

	return len;
}
