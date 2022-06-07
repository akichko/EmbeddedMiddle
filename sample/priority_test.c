/*
 sudo taskset -c X(CPU番号) sample/priority_test 等で実行
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "../src/em_task.h"

#define TASK_ID_LOW 100
#define TASK_ID_HIGH 200

int threadfunc_low();
int threadfunc_high();

em_tasksetting_t tasklist[] = {
    {"LowPriority", TASK_ID_LOW, 3, 0, 5, NULL, threadfunc_low},
    {"HighPriority", TASK_ID_HIGH, 5, 0, 5, NULL, threadfunc_high}};

em_taskmng_t tm;

int main(int argc, char **argv)
{
    int ret;
    int task_num = sizeof(tasklist) / sizeof(em_tasksetting_t);
    printf("task num %d\n", task_num);

    if (0 != em_init_taskmng(&tm, task_num, sizeof(int)))
    {
        printf("em_init_taskmng error\n");
        exit(1);
    }

    for (int i = 0; i < task_num; i++)
    {
        if (0 != em_task_create(&tm, tasklist[i]))
        {
            printf("em_task_create error [%d]\n", i);
            exit(1);
        }
    }
    while (1)
    {
        sleep(10);
    }
    return 0;
}

int threadfunc_low()
{
    int i = 0;
    while (1)
    {
        printf("[Low] %d\n", i++);
        usleep(100 * 1000);
    }
}

int threadfunc_high()
{
    while (1)
    {
        printf("[Hight] Calc Start\n");
        for (long i = 0; i < 2000*1000*1000; i++)
        {
            i++;
        }
        printf("[Hight] Calc End\n");

        sleep(1);
    }
}
