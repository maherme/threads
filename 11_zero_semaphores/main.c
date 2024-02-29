#include <stdlib.h>
#include <stdio.h>
#include "threads.h"

#define MAX_COUNTER 14

sem_t sem0_1, sem0_2;

static void *
fn_callback1(void *arg)
{
    char *th_name = (char *)arg;

    for(int i = 1; i < MAX_COUNTER + 1; i += 2)
    {
        printf("%s: Count = %d\n", th_name, i);
        semaphore_post(&sem0_2);
        semaphore_wait(&sem0_1);
    }

    return NULL;
}

static void *
fn_callback2(void *arg)
{
    char *th_name = (char *)arg;

    for(int i = 2; i < MAX_COUNTER + 1; i += 2)
    {
        semaphore_wait(&sem0_2);
        printf("%s: Count = %d\n", th_name, i);
        semaphore_post(&sem0_1);
    }

    return NULL;
}

int main()
{
    pthread_t th1, th2;
    const char *th1_name = "TH1";
    const char *th2_name = "TH2";

    semaphore_init(&sem0_1, 0, 0);
    semaphore_init(&sem0_2, 0, 0);

    thread_create(&th1, fn_callback1, th1_name);
    thread_create(&th2, fn_callback2, th2_name);

    thread_join(th1);
    thread_join(th2);

    semaphore_destroy(&sem0_1);
    semaphore_destroy(&sem0_2);

    printf("Program Finished\n");
    exit(EXIT_SUCCESS);
}
