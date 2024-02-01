#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "queue.h"
#include "threads.h"

queue_t *
init_queue(void)
{
    queue_t *q = calloc(1, sizeof(queue_t));

    q->rear = 0;
    q->front = q->rear;
    thread_mutex_init(&q->mutex);
    thread_cond_init(&q->cv);
    q->count = 0;

    return q;
}

bool
is_queue_empty(queue_t *q)
{
    if(0 == q->count)
    {
        return true;
    }

    return false;
}

bool
is_queue_full(queue_t *q)
{
    if(Q_DEFAULT_SIZE == q->count)
    {
        return true;
    }

    return false;
}

bool
enqueue(queue_t *q, void *ptr)
{
    if(!q || !ptr)
    {
        return false;
    }

    if(is_queue_full(q))
    {
        printf("Queue is full\n");
        return false;
    }

    if(is_queue_empty(q))
    {
        q->elem[q->front] = ptr;
        q->count++;
        return true;
    }

    /* rear can overflow, so make circular */
    q->rear = (q->rear + 1) % Q_DEFAULT_SIZE;

    q->elem[q->rear] = ptr;
    q->count++;

    return true;
}

void *
dequeue(queue_t *q)
{
    void *elem = NULL;

    if(!q)
    {
        return NULL;
    }

    if(is_queue_empty(q))
    {
        return NULL;
    }

    elem = q->elem[q->front];
    q->elem[q->front] = NULL;
    q->count--;

    /* last element */
    if(q->front == q->rear)
    {
        return elem;
    }

    /* front can overflow, so make circular */
    q->front = (q->front + 1) % Q_DEFAULT_SIZE;

    return elem;
}

void
print_queue(queue_t *q)
{
    printf("q->front = %d, q->rear = %d, q->count = %d\n", q->front, q->rear, q->count);

    for(int i = 0; i < Q_DEFAULT_SIZE; i++)
    {
        if(q->elem[i] == NULL)
        {
            continue;
        }
        printf("index = %u, elem = %p\n", i, q->elem[i]);
    }
}
