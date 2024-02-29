#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "semaphores.h"
#include "threads.h"

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

semaphore_t *
semaphore_create(void)
{
    semaphore_t *sem = calloc(1, sizeof(semaphore_t));

    if(!sem)
    {
        handle_error("calloc");
    }

    return sem;
}

void
semaphore_init(semaphore_t *sem, int count)
{
    sem->permit_counter = count;
    thread_cond_init(&sem->cv);
    thread_mutex_init(&sem->mutex);
}

void
semaphore_wait(semaphore_t *sem)
{
    thread_mutex_lock(&sem->mutex);
    sem->permit_counter--;
    if(0 > sem->permit_counter)
    {
        thread_cond_wait(&sem->cv, &sem->mutex);
    }
    thread_mutex_unlock(&sem->mutex);
}

void
semaphore_post(semaphore_t *sem)
{
    bool any_thread_waiting;

    thread_mutex_lock(&sem->mutex);
    any_thread_waiting = sem->permit_counter < 0 ? true : false;
    sem->permit_counter++;
    if(any_thread_waiting)
    {
        thread_cond_signal(&sem->cv);
    }
    thread_mutex_unlock(&sem->mutex);
}

void
semaphore_destroy(semaphore_t *sem)
{
    sem->permit_counter = 0;
    thread_cond_destroy(&sem->cv);
    thread_mutex_destroy(&sem->mutex);
}

int
semaphore_getvalue(semaphore_t *sem)
{
    return sem->permit_counter;
}
