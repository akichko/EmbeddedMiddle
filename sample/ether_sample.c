#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../src/em_ether.h"
#include "../src/em_queue.h"

em_socket_t sock_tx;
em_socket_t sock_txq; // tx main
em_socket_t sock_rx;
em_socket_t sock_rxq;

em_queue_t queue_tx;
em_queue_t queue_rx;

int callback(em_ethpacket_t *packet)
{
    printf("[Rx] Receive: %s [length:%d]\n", packet->data, packet->length);
    return 0;
}

void *routineTx(void *p)
{
    static int cnt = 1000;
    em_ethpacket_t packet;

    while (1)
    {
        // printf("[Tx] Send Q\n");
        if (0 != em_dequeue(&queue_tx, &packet, 1000))
        {
            printf("[Tx] dequque error\n");
            continue;
        }

        printf("[Tx] Send: %s [length:%d] QueueTx -> Rx2\n", packet.data, packet.length);
        if (0 != em_udp_send(&sock_txq, &packet, 1000))
        {
            printf("[Tx] send error\n");
            continue;
        }

        sprintf((char *)packet.data, "Hello %d from Tx", cnt++);
        packet.length = strlen((char *)packet.data);
        printf("[Tx] Send: %s [length:%d] -> Rx\n", packet.data, packet.length);

        if (0 != em_udp_send(&sock_tx, &packet, 300))
        {
            printf("[Tx] send error\n");
        }

        sleep(1);
    }
    return (NULL);
}

void *routineRx(void *p)
{
    em_ethpacket_t packet;
    // int data_size;

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
    em_ethpacket_t packet;
    // int (*callback_func)(em_ethpacket_t *) = callback;
    while (1)
    {
        if (0 != em_udp_recv(&sock_rxq, &packet, EM_NO_TIMEOUT))
        {
            printf("[Rx2] receive error\n");
            continue;
        }
        // printf("[Rx2] receive Q\n");
        printf("[Rx2] Receive: %s [length:%d] -> QueueRx\n", packet.data, packet.length);

        if (0 != em_enqueue(&queue_rx, &packet, EM_NO_TIMEOUT))
        {
            printf("[Rx2] enqueue error\n");
            continue;
        }
    }
    return (NULL);
}

int main(void)
{
    static int cnt = 0;
    pthread_t p1, p2, p3;

    em_udp_tx_init(&sock_tx, "127.0.0.2", 20000, "127.0.0.1", 23456);
    em_udp_rx_init(&sock_rx, "0.0.0.0", 23456);
    em_udp_tx_init(&sock_txq, "0.0.0.0", 19999, "127.0.0.1", 34567);
    em_udp_rx_init(&sock_rxq, "0.0.0.0", 34567);

    em_queue_create(&queue_tx, sizeof(em_ethpacket_t), 10, &malloc, &free);
    em_queue_create(&queue_rx, sizeof(em_ethpacket_t), 10, &malloc, &free);

    pthread_create(&p1, NULL, &routineRx, NULL);
    pthread_create(&p2, NULL, &routineTx, NULL);
    pthread_create(&p3, NULL, &routineRx2, NULL);

    em_ethpacket_t packet;
    while (1)
    {
        sprintf((char *)packet.data, "Queue %d from Main", cnt++);
        packet.length = strlen((char *)packet.data);
        printf("[Main] Send: %s [length:%d] -> QueueTx\n", packet.data, packet.length);

        if (0 != em_enqueue(&queue_tx, &packet, EM_NO_WAIT))
        {
            printf("[Main] send Q error\n");
        }

        if (0 == em_dequeue(&queue_rx, &packet, EM_NO_TIMEOUT))
        {
            printf("[Main] Receive Q: %s [length:%d] <- QueueRx\n", packet.data, packet.length);
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
