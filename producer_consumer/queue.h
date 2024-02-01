#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdbool.h>

#define Q_DEFAULT_SIZE  5

typedef struct
{
    int elem[Q_DEFAULT_SIZE];
    unsigned int front;
    unsigned int rear;
    unsigned int count;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
} queue_t;

queue_t *
init_queue(void);

bool
is_queue_empty(queue_t *q);

bool
is_queue_full(queue_t *q);

bool
enqueue(queue_t *q, int elem);

int
dequeue(queue_t *q, int *result);

void
print_queue(queue_t *q);

#endif /* QUEUE_H */
