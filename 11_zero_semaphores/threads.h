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
thread_cond_init(pthread_cond_t *cond);

void
thread_cond_signal(pthread_cond_t *cond);

void
thread_cond_broadcast(pthread_cond_t *cond);

void
thread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);

void
semaphore_init(sem_t *sem, int pshared, unsigned int value);

void
semaphore_destroy(sem_t *sem);

void
semaphore_wait(sem_t *sem);

void
semaphore_post(sem_t *sem);

#endif /* THREADS_H */
