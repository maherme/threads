#ifndef SEMAPHORES_H
#define SEMAPHORES_H

#include <pthread.h>

typedef struct
{
    int permit_counter;
    pthread_cond_t cv;
    pthread_mutex_t mutex;
}semaphore_t;

semaphore_t *
semaphore_create(void);

void
semaphore_init(semaphore_t *sem, int count);

void
semaphore_wait(semaphore_t *sem);

void
semaphore_post(semaphore_t *sem);

void
semaphore_destroy(semaphore_t *sem);

int
semaphore_getvalue(semaphore_t *sem);

#endif /* SEMAPHORES_H */
