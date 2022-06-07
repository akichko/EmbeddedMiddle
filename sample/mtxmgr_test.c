#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "../src/em_mtxmng.h"

em_mtxmng_t mtxmng;
int g_mutex_id;

void *mutex_func(void *p)
{
    sleep(1);
    while (1)
    {
        if (0 != em_mtxmng_lock(&mtxmng, g_mutex_id, EM_NO_TIMEOUT))
        {
            printf("[Sub] lock error\n");
        }
        printf("[Sub] locked\n");

        sleep(1);

        em_mtxmng_unlock(&mtxmng, g_mutex_id);
        printf("[Sub] unlocked\n");

        sleep(1);
    }
}

int main(void)
{
    pthread_t p1;
    int mutex1, mutex2, mutex3, mutex4;

    em_mtxmng_init(&mtxmng, 5);

    mutex1 = em_mtxmng_create_mutex(&mtxmng);
    printf("mutex1_id = %d\n", mutex1);

    mutex2 = em_mtxmng_create_mutex(&mtxmng);
    printf("mutex2_id = %d\n", mutex2);

    mutex3 = em_mtxmng_create_mutex(&mtxmng);
    printf("mutex3_id = %d\n", mutex3);

    em_mtxmng_delete_mutex(&mtxmng, mutex2);
    printf("mutex2 deleted (id=%d)\n", mutex2);

    mutex4 = em_mtxmng_create_mutex(&mtxmng);
    printf("mutex4_id = %d\n", mutex4);

    g_mutex_id = em_mtxmng_create_mutex(&mtxmng);
    printf("g_mutex_id = %d\n", g_mutex_id);

    sleep(1);

    pthread_create(&p1, NULL, &mutex_func, NULL);

    int i = 0;
    while (1)
    {
        if (0 != em_mtxmng_lock(&mtxmng, g_mutex_id, 400))
        {
            printf("[Main] lock error\n");
            continue;
        }
        printf("[Main] %d\n", i++);
        em_mtxmng_unlock(&mtxmng, g_mutex_id);
        
        usleep(200 * 1000);
    }

    pthread_join(p1, NULL);
}
