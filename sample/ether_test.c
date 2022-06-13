#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "../src/em_ether.h"
#include "../src/em_queue.h"

em_socket_t sock_tx;
em_socket_t sock_tx2;
em_socket_t sock_rx;
em_socket_t sock_rx2;

int callback(em_ethpacket_t *packet)
{
    printf("[Rx] Receive: %s [length:%d]\n", packet->data, packet->length);
    return 0;
}

void *routineTx(void *p)
{
    static int cnt = 10000;
    em_ethpacket_t packet;

    while (1)
    {
        printf("[Tx] Send Q\n");
        if (0 != em_udp_send_dequeue(&sock_tx2, 1000))
        {
            printf("[Tx] Q error\n");
            continue;
        }
        else
        {
            printf("[Tx] Send Q success\n");
        }
#if 1
        sprintf((char *)packet.data, "Hello %d", cnt++);
        packet.length = strlen((char*)packet.data);
        printf("[Tx] Send: %s [length:%d]\n", packet.data, packet.length);

        if (0 != em_udp_send(&sock_tx, &packet, 300))
        {
            printf("[Tx] send error\n");
        }
#endif
        sleep(1);
    }
    return (NULL);
}

void *routineRx(void *p)
{
    em_ethpacket_t packet;
    //int data_size;

    int (*callback_func)(em_ethpacket_t *) = callback;
    while (1)
    {
        if (0 != em_udp_recv(&sock_rx, &packet, EM_NO_TIMEOUT))
        {
            printf("[Rx] error\n");
            continue;
        }
        callback_func(&packet);
    }
    return (NULL);
}

void *routineRx2(void *p)
{
   // em_ethpacket_t packet;
   // int (*callback_func)(em_ethpacket_t *) = callback;
    while (1)
    {
        if (0 != em_udp_recv_enqueue(&sock_rx2, EM_NO_TIMEOUT))
        {
            printf("[Rx2] error\n");
            continue;
        }
            printf("[Rx2] receive Q\n");
    }
    return (NULL);
}

int main(void)
{
    static int cnt = 0;
    pthread_t p1, p2, p3;

    em_udp_tx_init(&sock_tx, "127.0.0.1", 23456, 10);
    em_udp_rx_init(&sock_rx, "0.0.0.0", 23456, 10);
    em_udp_tx_init(&sock_tx2, "127.0.0.1", 34567, 10);
    em_udp_rx_init(&sock_rx2, "0.0.0.0", 34567, 10);

    pthread_create(&p1, NULL, &routineRx, NULL);
    pthread_create(&p2, NULL, &routineTx, NULL);
    pthread_create(&p3, NULL, &routineRx2, NULL);

    em_ethpacket_t packet;
    while (1)
    {
        sprintf((char*)packet.data, "Hello Q %d", cnt++);
        packet.length = strlen((char*)packet.data);
        printf("[Main] Send: %s [length:%d]\n", packet.data, packet.length);

        if (0 != em_udp_send_enqueue(&sock_tx2, &packet, EM_NO_WAIT))
        {
            printf("[Main] send Q error\n");
        }

        if (0 == em_udp_recv_dequeue(&sock_rx2, &packet, EM_NO_TIMEOUT))
        {
            printf("[Main] Receive Q: %s [length:%d]\n", packet.data, packet.length);
        }
        else
        {
            printf("[Main] Receive Q error\n");
        }
        sleep(1);
    }

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
}
