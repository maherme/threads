#ifndef THREAD_BARRIER_H
#define THREAD_BARRIER_H

#include <stdbool.h>
#include <pthread.h>

typedef struct
{
    unsigned int threshold_count;
    unsigned int current_wait_count;
    pthread_cond_t cv;
    pthread_mutex_t mutex;
    bool is_ready_again;
    pthread_cond_t busy_cv;
}thread_barrier_t;

void
thread_barrier_init(thread_barrier_t *barrier, unsigned int threshold_count);

void
thread_barrier_wait(thread_barrier_t *barrier);

void
thread_barrier_signal_all(thread_barrier_t *barrier);

void
thread_barrier_destroy(thread_barrier_t *barrier);

void
thread_barrier_print(thread_barrier_t *barrier);

#endif /* THREAD_BARRIER_H */
