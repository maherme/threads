#ifndef MONITOR_RW_H
#define MONITOR_RW_H

#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>

#define MONITOR_RW_MAX_READER_THREADS_DEF  0xFFFF
#define MONITOR_RW_MAX_WRITER_THREADS_DEF  1

typedef struct {
    pthread_mutex_t state_mutex;    /**< @brief To access state of monitor in a mutually exclusive way */
    pthread_cond_t cv_reader;       /**< @brief Blocks the reader when the lock is not available */
    pthread_cond_t cv_writer;       /**< @brief Blocks the writer when the lock is not available */
    bool dont_allow_readers;        /**< @brief Readers are not allowed to grab the monitor (false) */
    bool dont_allow_writers;        /**< @brief Writers are not allowed to grab the monitor (false) */
    int n_locks;                    /**< @brief Number of threads in the critical section */
    int n_reader_waiting;           /**< @brief Number of threads waiting for the lock */
    int n_writer_waiting;           /**< @brief Number of threads waiting for the lock */
    bool is_locked_by_reader;       /**< @brief The lock is currently occupied by a reader */
    bool is_locked_by_writer;       /**< @brief The lock is currently occupied by a writer */
    uint16_t n_max_readers;         /**< @brief Max number of readers allowed to enter in the C.S. */
    uint16_t n_max_writers;         /**< @brief Max number of writers allowed to enter in the C.S. */
} monitor_rw_t;

void
monitor_rw_init(monitor_rw_t *monitor_rw);

void
monitor_rw_set_max_readers_writers(monitor_rw_t *monitor_rw, uint16_t max_readers, uint16_t max_writers);

void
monitor_rw_rd_lock(monitor_rw_t *monitor_rw);

void
monitor_rw_wr_lock(monitor_rw_t *monitor_rw);

void
monitor_rw_unlock(monitor_rw_t *monitor_rw);

void
monitor_rw_destroy(monitor_rw_t *monitor_rw);

#endif /* MONITOR_RW_H */
