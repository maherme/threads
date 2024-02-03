#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "thread_barrier.h"
#include "threads.h"

#define NUMBER_THREADS  3

static thread_barrier_t thread_barrier;

void *
fn_callback(void *arg)
{
    thread_barrier_wait(&thread_barrier);
    printf("1st barricade cleared by thread %s\n", (char*)arg);

    thread_barrier_wait(&thread_barrier);
    printf("2nd barricade cleared by thread %s\n", (char*)arg);

    thread_barrier_wait(&thread_barrier);
    printf("3rd barricade cleared by thread %s\n", (char*)arg);

    pthread_exit(NULL);
}

int
main()
{
    static const char *threads_id[NUMBER_THREADS] = {"th1", "th2", "th3"};
    static pthread_t pthreads[NUMBER_THREADS];

    thread_barrier_init(&thread_barrier, NUMBER_THREADS);

    for(int i = 0; i < NUMBER_THREADS; i++)
    {
        thread_create(&pthreads[i], fn_callback, (void*)threads_id[i]);
    }

    for(int i = 0; i < NUMBER_THREADS; i++)
    {
        thread_join(pthreads[i]);
    }

    thread_barrier_print(&thread_barrier);
    exit(EXIT_SUCCESS);
}
