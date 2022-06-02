#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "em_ether.h"
#include "em_queue.h"
#include "em_timer.h"

int em_udp_tx_init(em_socket_t *sk, char *dest_ip, uint16_t dest_port, int queue_size)
{
	sk->sock = socket(AF_INET, SOCK_DGRAM, 0);
	sk->addr.sin_family = AF_INET;
	sk->addr.sin_port = htons(dest_port);
	sk->addr.sin_addr.s_addr = inet_addr(dest_ip);
	em_queue_create(&sk->queue, sizeof(em_ethpacket_t), queue_size);
	return 0;
}

int em_udp_rx_init(em_socket_t *sk, char *ip_from, uint16_t local_port, int queue_size)
{
	sk->sock = socket(AF_INET, SOCK_DGRAM, 0);
	sk->addr.sin_family = AF_INET;
	sk->addr.sin_port = htons(local_port);
	sk->addr.sin_addr.s_addr = inet_addr(ip_from);
	em_queue_create(&sk->queue, sizeof(em_ethpacket_t), queue_size);
	bind(sk->sock, (struct sockaddr *)&sk->addr, sizeof(sk->addr));
	return 0;
}

int em_udp_send(em_socket_t *sk, em_ethpacket_t *packet, int timeout_ms)
{
	sendto(sk->sock, packet->data, packet->length, 0,
		   (struct sockaddr *)&sk->addr, sizeof(sk->addr));
	return 0;
}

int em_udp_send_enqueue(em_socket_t *sk, em_ethpacket_t *packet, int timeout_ms)
{
	if (0 != em_enqueue(&sk->queue, packet, timeout_ms))
		printf("error em_udp_send_enqueue\n");

	return 0;
}

int em_udp_send_dequeue(em_socket_t *sk, int timeout_ms)
{
	em_ethpacket_t packet;
	if (0 != em_dequeue(&sk->queue, &packet, timeout_ms))
	{
		printf("error em_udp_send_dequeue\n");
		return -1;
	}
	sendto(sk->sock, packet.data, packet.length, 0,
		   (struct sockaddr *)&sk->addr, sizeof(sk->addr));
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
			printf("em_udp_recv timeout [%ds]\n", timeout_ms);
		}
	}

	memset(packet, 0, sizeof(*packet));
	packet->length = recv(sk->sock, packet->data, sizeof(packet->data), 0);
	return 0;
}

int em_udp_recv_enqueue(em_socket_t *sk, int timeout_ms)
{
	em_ethpacket_t packet;
	if(0 != em_udp_recv(sk, &packet, timeout_ms)){
		printf("error em_udp_recv_enqueue\n");
		return -1;
	}

	if (0 != em_enqueue(&sk->queue, &packet, EM_NO_WAIT))
		printf("error em_udp_recv_enqueue\n");

	return 0;
}

int em_udp_recv_dequeue(em_socket_t *sk, em_ethpacket_t *packet, int timeout_ms)
{
	if (0 != em_dequeue(&sk->queue, packet, timeout_ms))
	{
		printf("error em_udp_recv_dequeue\n");
		return -1;
	}
	return 0;
}
