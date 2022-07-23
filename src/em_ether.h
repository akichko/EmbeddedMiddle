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
				   const char *dest_ip,
				   const uint16_t dest_port,
				   int queue_size);

int em_udp_rx_init(em_socket_t *sk,
				   const char *ip_from, //送信元フィルタ
				   const uint16_t local_port,
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
