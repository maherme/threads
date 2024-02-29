#ifndef RW_LOCKS_H
#define RW_LOCKS_H

#include <pthread.h>
#include <stdbool.h>

typedef struct {
    pthread_mutex_t state_mutex;
    pthread_cond_t cv;
    int n_locks;
    int n_reader_waiting;
    int n_writer_waiting;
    bool is_locked_by_reader;
    bool is_locked_by_writer;
    pthread_t writer_thread;
} rw_lock_t;

void
rw_lock_init(rw_lock_t *rw_lock);

void
rw_lock_rd_lock(rw_lock_t *rw_lock);

void
rw_lock_wr_lock(rw_lock_t *rw_lock);

void
rw_lock_unlock(rw_lock_t *rw_lock);

void
rw_lock_destroy(rw_lock_t *rw_lock);

#endif /* RW_LOCKS_H */
