#ifndef __EM_ETHER_H__
#define __EM_ETHER_H__

#include <arpa/inet.h>
#include "em_queue.h"

typedef struct
{
	int length;
	unsigned char data[1500];
} em_ethpacket_t;

typedef struct
{
	int sock;
	int socket_type;
	struct sockaddr_in addr;
	em_queue_t queue;
} em_socket_t;

int em_udp_tx_init(em_socket_t *sk,
				   char *dest_ip,
				   uint16_t dest_port,
				   int queue_size);

int em_udp_rx_init(em_socket_t *sk,
				   char *ip_from, //送信元フィルタ
				   uint16_t local_port,
				   int queue_size);

int em_udp_send(em_socket_t *sk,
				em_ethpacket_t *packet,
				int timeout_ms);

int em_udp_send_enqueue(em_socket_t *sk,
						em_ethpacket_t *packet,
						int timeout_ms);

int em_udp_send_dequeue(em_socket_t *sk,
						int timeout_ms);

int em_udp_recv(em_socket_t *sk,
				em_ethpacket_t *packet,
				int timeout_ms);

int em_udp_recv_enqueue(em_socket_t *sk,
						int timeout_ms);

int em_udp_recv_dequeue(em_socket_t *sk,
						em_ethpacket_t *packet,
						int timeout_ms);

#endif //__EM_ETHER_H__
