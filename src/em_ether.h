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
#include "em_cmndefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum _em_ethertype_t
{
	EM_ETHER_TYPE_UDP = 1,
	EM_ETHER_TYPE_TCP = 2,
	
} em_ethertype_t;

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
	char is_connected;
} em_socket_t;

int em_socket_init(em_socket_t *sk,
				   em_ethertype_t type,
				   const char *local_ip,
				   const ushort local_port,
				   char is_reuseaddr);

void em_socket_close(em_socket_t *sk);

int em_socket_set_destination(em_socket_t *sk,
							  const char *dest_ip,
							  const ushort dest_port);

//UDP

int em_udp_tx_init(em_socket_t *sk,
				   const char *local_ip, //"0.0.0.0"ならIP自動設定
				   const ushort local_port, //0ならポートを自動設定
				   const char *dest_ip,
				   const ushort dest_port);

int em_udp_rx_init(em_socket_t *sk,
				   const char *local_ip,
				   const ushort local_port);

int em_udp_send(em_socket_t *sk,
				em_ethpacket_t *packet,
				int timeout_ms);

int em_udp_send2(em_socket_t *sk,
				em_ethpacket_t *packet,
				const char *dest_ip, 
				const ushort dest_port,
				int timeout_ms);

int em_udp_recv(em_socket_t *sk,
				em_ethpacket_t *packet,
				int timeout_ms);

//TCP

int em_socket_init_tcp(em_socket_t *sk,
					   const char *local_ip,
					   const ushort local_port);

int em_tcp_connect_client(em_socket_t *sk,
						  const char *local_ip,		 //"0.0.0.0"ならIP自動設定
						  const ushort local_port, // 0ならポートを自動設定
						  const char *dest_ip,
						  const ushort dest_port,
						  char is_reuseaddr,
						  int timeout_ms);

int em_tcp_connect_server(em_socket_t *sk_listen,
						  em_socket_t *sk_client, //out
						  const char *local_ip, //"0.0.0.0"ならIP自動設定
						  const ushort local_port,
						  char is_reuseaddr,
						  int timeout_ms);

int em_tcp_disconnect(em_socket_t *sk);

int em_tcp_send(em_socket_t *sk,
				unsigned char *send_buf,
				int size,
				int timeout_ms);

int em_tcp_recv(em_socket_t *sk,
				unsigned char *recv_buf,
				int size,
				int timeout_ms);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif //__EM_ETHER_H__
