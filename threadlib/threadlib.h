#ifndef THREADLIB_H
#define THREADLIB_H

#include <pthread.h>
#include <semaphore.h>
#include "glthreads.h"

/**
 * @defgroup Thread_State Thread possible states
 * @{
 */
/** @brief Thread running normally */
#define THREAD_F_RUNNING            (1 << 0)
/** @brief Thread has been marked to pause, but not paused */
#define THREAD_F_MARKED_FOR_PAUSE   (1 << 1)
/** @brief Thread is blocked (paused) */
#define THREAD_F_PAUSED             (1 << 2)
/** @brief Thread is blocked on cv for reason other than paused */
#define THREAD_F_BLOCKED            (1 << 3)
/**@}*/

typedef struct
{
    char name[32];                      /**< @brief thread name */
    bool thread_created;                /**< @brief if execution unit has been created */
    pthread_t thread;                   /**< @brief thread handle */
    void *arg;                          /**< @brief thread fn arguments */
    void *(*thread_fn)(void *);         /**< @brief thread fn */
    pthread_attr_t attributes;          /**< @brief thread attributes */
    void *(*thread_pause_fn)(void *);   /**< @brief fn to be called just before pausing the thread */
    void *pause_arg;                    /**< @brief arguments of thread_pause_fn */
    unsigned int flags;                 /**< @brief for tracking thread state */
    pthread_mutex_t state_mutex;        /**< @brief update thread state mutually exclusively */
    pthread_cond_t cv;                  /**< @brief cv on which thread will be blocked itself */
    sem_t *semaphore;
    glthread_node_t glnode;             /**< @brief struct node of the pool thread glued list */
}thread_t;

typedef struct
{
    glthread_t pool_list;
    pthread_mutex_t mutex;
}thread_pool_t;

typedef struct
{
    int thread_wait_count;          /**< @brief number of threads waiting in a wait queue */
    pthread_cond_t cv;              /**< @brief cv on which threads in wait queue are blocked */
    pthread_mutex_t *app_mutex;     /**< @brief application owned mutex cached by wait queue */
}wait_queue_t;

/** @brief Pointer to condition function for wait queue */
typedef bool (*wait_queue_cond_fn) (void *app_arg, pthread_mutex_t **out_mutex);

/**********************************************************************************************************/
/*                                      Basic APIs                                                        */
/**********************************************************************************************************/

void
thread_attr_init(pthread_attr_t *attr);

void
thread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

void
thread_create(pthread_t *pthread_handle,
              pthread_attr_t *attr,
              void *(*start_routine)(void *),
              const void *thread_id);

void
thread_join(pthread_t thread_handler);

void
thread_mutex_init(pthread_mutex_t *mutex);

void
thread_mutex_lock(pthread_mutex_t *mutex);

void
thread_mutex_unlock(pthread_mutex_t *mutex);

void
thread_mutex_destroy(pthread_mutex_t *mutex);

void
thread_spin_init(pthread_spinlock_t *spinlock);

void
thread_spin_lock(pthread_spinlock_t *spinlock);

void
thread_spin_unlock(pthread_spinlock_t *spinlock);

void
thread_spin_destroy(pthread_spinlock_t *spinlock);

void
thread_cond_init(pthread_cond_t *cond);

void
thread_cond_signal(pthread_cond_t *cond);

void
thread_cond_broadcast(pthread_cond_t *cond);

void
thread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);

void
thread_cond_destroy(pthread_cond_t *cond);

void
thread_rwlock_init(pthread_rwlock_t *rw_lock);

void
thread_rwlock_rdlock(pthread_rwlock_t *rw_lock);

void
thread_rwlock_wrlock(pthread_rwlock_t *rw_lock);

void
thread_rwlock_unlock(pthread_rwlock_t *rw_lock);

void
thread_rwlock_destroy(pthread_rwlock_t *rw_lock);

void
semaphore_init(sem_t *sem, int pshared, unsigned int value);

void
semaphore_destroy(sem_t *sem);

void
semaphore_wait(sem_t *sem);

void
semaphore_post(sem_t *sem);

/**********************************************************************************************************/
/*                                      Complex APIs                                                      */
/**********************************************************************************************************/

thread_t *
thread_init(thread_t *thread, char *name);

void
thread_run(thread_t *thread, void *(*thread_fn)(void *), void *arg);

void
thread_set_pause_fn(thread_t *thread, void *(*thread_pause_fn)(void *), void *pause_arg);

void
thread_pause(thread_t *thread);

void
thread_resume(thread_t *thread);

void
thread_test_and_pause(thread_t *thread);

void
thread_pool_init(thread_pool_t *thread_pool);

void
thread_pool_insert_new_thread(thread_pool_t *thread_pool, thread_t *thread);

thread_t *
thread_pool_get_thread(thread_pool_t *thread_pool);

void
thread_pool_dispatch_thread(thread_pool_t *thread_pool,
                            void *(*thread_fn)(void *),
                            void *arg,
                            bool block_caller);

void
wait_queue_init(wait_queue_t *wq);

thread_t *
wait_queue_test_and_wait(wait_queue_t *wq, wait_queue_cond_fn wait_queue_cond_fn_cb, void *arg);

void
wait_queue_signal(wait_queue_t *wq, bool lock_mutex);

void
wait_queue_broadcast(wait_queue_t *wq, bool lock_mutex);

void
wait_queue_destroy(wait_queue_t *wq);

#endif /* THREADLIB_H */
