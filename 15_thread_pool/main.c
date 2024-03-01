#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "threadlib.h"

void *
even_thread_work_reverse(void *arg)
{
    (void)arg;

    for(int i = 10; i > 0; i -= 2)
    {
        printf("even = %d\n", i);
        sleep(1);
    }

    return NULL;
}

void *
odd_thread_work_reverse(void *arg)
{
    (void)arg;

    for(int i = 9; i > 0; i -= 2)
    {
        printf("odd = %d\n", i);
        sleep(1);
    }

    return NULL;
}

void *
even_thread_work(void *arg)
{
    (void)arg;

    for(int i = 0; i < 10; i += 2)
    {
        printf("even = %d\n", i);
        sleep(1);
    }

    return NULL;
}

void *
odd_thread_work(void *arg)
{
    (void)arg;

    for(int i = 1; i < 10; i += 2)
    {
        printf("odd = %d\n", i);
        sleep(1);
    }

    return NULL;
}

int
main()
{
    thread_pool_t *thread_pool = calloc(1, sizeof(thread_pool_t));
    thread_pool_init(thread_pool);

    thread_t *thread1 = thread_init(NULL, "even_thread");
    thread_t *thread2 = thread_init(NULL, "odd_thread");

    thread_pool_insert_new_thread(thread_pool, thread1);
    thread_pool_insert_new_thread(thread_pool, thread2);

    thread_pool_dispatch_thread(thread_pool, even_thread_work, 0, true);
    thread_pool_dispatch_thread(thread_pool, odd_thread_work, 0, true);

    sleep(20);

    printf("Dispatching threads with new application work\n");
    thread_pool_dispatch_thread(thread_pool, even_thread_work_reverse, 0, true);
    thread_pool_dispatch_thread(thread_pool, odd_thread_work_reverse, 0, true);

    pthread_exit(NULL);
    exit(EXIT_SUCCESS);
}
