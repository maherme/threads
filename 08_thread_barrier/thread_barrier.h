#ifndef THREAD_BARRIER_H
#define THREAD_BARRIER_H

#include <stdbool.h>
#include <pthread.h>

typedef struct
{
    unsigned int threshold_count;       /**< @brief Maximum number of blocked threads */
    unsigned int current_wait_count;    /**< @brief Current number of blocked threads */
    pthread_cond_t cv;                  /**< @brief CV where threads will be blocked */
    pthread_mutex_t mutex;              /**< @brief For managing the mutual exclusion of the data struct */
    bool is_ready_again;                /**< @brief Tracks if the barrier dispositon is on going (false) */
    pthread_cond_t busy_cv;             /**< @brief Blocks arriving threads if disposition is on going */
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
