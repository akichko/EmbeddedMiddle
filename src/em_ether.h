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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct tag_em_ethpacket
{
	int length;
	unsigned char data[1500];
	struct sockaddr_in src_addr;
	struct sockaddr_in dst_addr;
} em_ethpacket_t;

typedef struct
{
	int sock;
	int socket_type;
	struct sockaddr_in local_addr;
	struct sockaddr_in remote_addr;
	void *(*alloc_func)(size_t);
	void (*free_func)(void *);
} em_socket_t;

int em_udp_tx_init(em_socket_t *sk,
				   const char *local_ip, //NULLならIP・ポートを自動設定
				   const uint16_t local_port, //0ならIP・ポートを自動設定
				   const char *dest_ip,
				   const uint16_t dest_port,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_udp_rx_init(em_socket_t *sk,
				   const char *local_ip, //"0.0.0.0"なら自動設定
				   const uint16_t local_port,
				   void *(*alloc_func)(size_t),
				   void (*free_func)(void *));

int em_udp_send(em_socket_t *sk,
				em_ethpacket_t *packet,
				int timeout_ms);

int em_udp_recv(em_socket_t *sk,
				em_ethpacket_t *packet,
				int timeout_ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //__EM_ETHER_H__
