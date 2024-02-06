#ifndef RECURSIVE_MUTEX_H
#define RECURSIVE_MUTEX_H

#include <pthread.h>

typedef struct
{
    int n;
    pthread_t locking_thread;
    pthread_cond_t cv;
    pthread_mutex_t mutex;
    int threads_waiting;
}rec_mutex_t;

void
rec_mutex_init(rec_mutex_t *rec_mutex);

void
rec_mutex_lock(rec_mutex_t *rec_mutex);

void
rec_mutex_unlock(rec_mutex_t *rec_mutex);

void
rec_mutex_destroy(rec_mutex_t *rec_mutex);

#endif /* RECURSIVE_MUTEX_H */
