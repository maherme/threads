#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "threads.h"
#include "queue.h"

static queue_t *q;

static int
get_number(void)
{
    static int a = 0;
    a++;
    return a;
}

static void *
prod_fn(void *arg)
{
    size_t number;
    char *th_name = (char *)arg;

    printf("%s: Waiting to lock the queue\n", th_name);
    thread_mutex_lock(&q->mutex);
    printf("%s: Lock the queue\n", th_name);

    while(is_queue_full(q))
    {
        printf("%s: Blocks itself, Q is already full\n", th_name);
        thread_cond_wait(&q->cv, &q->mutex);
        printf("%s: Wakes up, checking the queue status again\n", th_name);
    }

    assert(is_queue_empty(q));

    while(!is_queue_full(q))
    {
        number = get_number();
        enqueue(q, number);
        printf("%s: Pushed integer %ld in queue, queue size = %d\n", th_name, number, q->count);
    }

    printf("%s: Filled up the queue, signalling and releasing lock\n", th_name);
    print_queue(q);
    thread_cond_broadcast(&q->cv);
    thread_mutex_unlock(&q->mutex);

    printf("%s: Finished and exit\n", th_name);
    return NULL;
}

static void *
cons_fn(void *arg)
{
    int number;
    char *th_name = (char *)arg;

    printf("%s: Waiting to lock the queue\n", th_name);
    thread_mutex_lock(&q->mutex);
    printf("%s: Lock the queue\n", th_name);

    while(is_queue_empty(q))
    {
        printf("%s: Blocks itself, Q is already empty\n", th_name);
        thread_cond_wait(&q->cv, &q->mutex);
        printf("%s: Wakes up, checking the queue status again\n", th_name);
    }

    assert(is_queue_full(q));

    while(!is_queue_empty(q))
    {
        if(dequeue(q, &number))
        {
            fprintf(stderr, "Error in dequeue\n");
            exit(EXIT_FAILURE);
        }
        printf("%s: Consumes an integer %d, queue size = %d\n", th_name, number, q->count);
    }

    printf("%s: Drains the entire queue, sending signal to blocking threads\n", th_name);
    thread_cond_broadcast(&q->cv);
    printf("%s: Releasing lock\n", th_name);
    thread_mutex_unlock(&q->mutex);

    printf("%s: Finished and exit\n", th_name);
    return NULL;
}

int
main(int argc, char **argv)
{
    const char *prod1 = "TP1";
    const char *prod2 = "TP2";
    const char *prod3 = "TP3";
    const char *cons1 = "TC1";
    const char *cons2 = "TC2";
    const char *cons3 = "TC3";
    pthread_t prod_th1, prod_th2, prod_th3;
    pthread_t cons_th1, cons_th2, cons_th3;

    q = init_queue();

    thread_create(&prod_th1, prod_fn, prod1);
    thread_create(&prod_th2, prod_fn, prod2);
    thread_create(&prod_th3, prod_fn, prod3);
    thread_create(&cons_th1, cons_fn, cons1);
    thread_create(&cons_th2, cons_fn, cons2);
    thread_create(&cons_th3, cons_fn, cons3);

    thread_join(prod_th1);
    thread_join(prod_th2);
    thread_join(prod_th3);
    thread_join(cons_th1);
    thread_join(cons_th2);
    thread_join(cons_th3);

    printf("Program Finished\n");

    pthread_exit(NULL);
}
