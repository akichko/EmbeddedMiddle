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
em_socket_t sock_rx;

int callback(em_ethpacket_t *packet)
{
    printf("Receive: %s [length:%d]\n", packet->data, packet->length);
}

void *routineTx(void *p)
{
    em_ethpacket_t packet;

    while (1)
    {
        if (0 != em_udp_send_dequeue(&sock_tx, EM_NO_TIMEOUT))
        {
            printf("error\n");
            continue;
        }
    }
    return (NULL);
}

void *routineRx(void *p)
{
    em_ethpacket_t packet;
    int data_size;

    int (*callback_func)(em_ethpacket_t *) = callback;
    while (1)
    {
        if (0 != em_udp_recv(&sock_rx, &packet, 1300))
        {
            printf("error\n");
            continue;
        }
        callback_func(&packet);
    }
    return (NULL);
}

int main(void)
{
    static int cnt = 0;
    pthread_t p1, p2;

    em_udp_tx_init(&sock_tx, "127.0.0.1", 23456, 10);
    em_udp_rx_init(&sock_rx, "0.0.0.0", 23456);

    pthread_create(&p1, NULL, &routineRx, NULL);
    pthread_create(&p2, NULL, &routineTx, NULL);

    em_ethpacket_t packet;
    while (1)
    {
        sprintf(packet.data, "Hello %d", cnt++);
        packet.length = strlen(packet.data);
        printf("Send: %s [length:%d]\n", packet.data, packet.length);

        if (0 != em_udp_send_enqueue(&sock_tx, &packet, 500))
        {
            printf("error\n");
        }
        sleep(1);
    }

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
}
