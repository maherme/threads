#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "threads.h"

#define NUMBER_THREADS                  5
#define SIZE_NAME_THREAD                8
#define CRITICAL_SECTION_LOOP_TIMES     5
#define PERMIT_THREADS_NUMBER           2

sem_t sem;

static void *
fn_callback(void *arg)
{
    char *thread_name = (char *)arg;

    printf("%s entering into critical section\n", thread_name);

    /* Critical section begins */
    semaphore_wait(&sem);
    printf("%s entered into critical section\n", thread_name);
    for(int i = 0;i < CRITICAL_SECTION_LOOP_TIMES; i++)
    {
        printf("%s executing in critical section\n", thread_name);
        sleep(1);
    }
    printf("%s exiting from critical section\n", thread_name);
    semaphore_post(&sem);
    /* Critical section ends */

    printf("%s exit from critical section\n", thread_name);

    free(arg);

    return NULL;
}

int main()
{
    pthread_t threads[NUMBER_THREADS];
    const char *threads_name[NUMBER_THREADS];

    semaphore_init(&sem, 0, PERMIT_THREADS_NUMBER);

    for(int i = 0; i < NUMBER_THREADS; i++)
    {
        char name[SIZE_NAME_THREAD];
        snprintf(name, sizeof(name), "Thread%d", i + 1);
        threads_name[i] = strdup(name);
    }

    for(int i = 0; i < NUMBER_THREADS; i++)
    {
        thread_create(&threads[i], fn_callback, threads_name[i]);
    }

    for(int i = 0; i < NUMBER_THREADS; i++)
    {
        thread_join(threads[i]);
    }

    semaphore_destroy(&sem);

    exit(EXIT_SUCCESS);
}
