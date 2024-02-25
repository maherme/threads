#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>
#include <semaphore.h>

void
thread_create(pthread_t *pthread_handle, void *(*start_routine)(void *), const void *thread_id);

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

#endif /* THREADS_H */
