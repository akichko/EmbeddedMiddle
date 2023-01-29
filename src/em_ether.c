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

int em_udp_tx_init(em_socket_t *sk, const char *local_ip, const uint16_t local_port,
				   const char *dest_ip, const uint16_t dest_port,
				   void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	sk->alloc_func = alloc_func;
	sk->free_func = free_func;
	sk->sock = socket(AF_INET, SOCK_DGRAM, 0);

	sk->remote_addr.sin_family = AF_INET;
	sk->remote_addr.sin_port = htons(dest_port);
	sk->remote_addr.sin_addr.s_addr = inet_addr(dest_ip);

	if (local_ip != NULL && local_port > 0)
	{
		sk->local_addr.sin_family = AF_INET;
		sk->local_addr.sin_port = htons(local_port);
		sk->local_addr.sin_addr.s_addr = inet_addr(local_ip);

		if (0 != bind(sk->sock, (struct sockaddr *)&sk->local_addr, sizeof(sk->local_addr)))
		{
			em_printf(EM_LOG_ERROR, "bind error\n");
			close(sk->sock);
			return -1;
		}
	}

	return 0;
}

int em_udp_rx_init(em_socket_t *sk, const char *local_ip, const uint16_t local_port, // uint queue_size,
				   void *(*alloc_func)(size_t), void (*free_func)(void *))
{
	sk->alloc_func = alloc_func;
	sk->free_func = free_func;
	sk->sock = socket(AF_INET, SOCK_DGRAM, 0);
	sk->local_addr.sin_family = AF_INET;
	sk->local_addr.sin_port = htons(local_port);
	sk->local_addr.sin_addr.s_addr = inet_addr(local_ip);
	// if (queue_size > 0)
	//{
	//	em_queue_create(&sk->queue, sizeof(em_ethpacket_t), queue_size, alloc_func, free_func);
	//	sk->has_queue = TRUE;
	// }
	// else
	//{
	//	sk->has_queue = FALSE;
	// }
	if (0 != bind(sk->sock, (struct sockaddr *)&sk->local_addr, sizeof(sk->local_addr)))
	{
		em_printf(EM_LOG_ERROR, "bind error\n");
		close(sk->sock);
		return -1;
	}
	return 0;
}

int em_udp_send(em_socket_t *sk, em_ethpacket_t *packet, int timeout_ms)
{
	sendto(sk->sock, packet->data, packet->length, 0,
		   (struct sockaddr *)&sk->remote_addr, sizeof(sk->remote_addr));
	return 0;
}

int em_udp_send2(em_socket_t *sk, em_ethpacket_t *packet, int timeout_ms,
				 const char *dest_ip, const uint16_t dest_port)
{
	struct sockaddr_in dst_addr = sk->remote_addr;

	if (dest_ip != NULL && dest_port > 0)
	{
		dst_addr.sin_family = AF_INET;
		dst_addr.sin_port = htons(dest_port);
		dst_addr.sin_addr.s_addr = inet_addr(dest_ip);
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
	socklen_t src_addr_len = sizeof(&packet->src_addr);

	int ret = recvfrom(sk->sock, packet->data, sizeof(packet->data), 0, (struct sockaddr *)&packet->src_addr, &src_addr_len);
	if (ret < 0)
	{
		return -1;
	}
	packet->length = ret;
	// printf("received from %s:%d\n", inet_ntoa(packet->src_addr.sin_addr), ntohs(packet->src_addr.sin_port));

	return 0;
}
