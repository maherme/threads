#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "threadlib.h"
#include "bitsop.h"

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define print_error_and_exit(msg) \
        do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while(0)

/**********************************************************************************************************/
/*                                      Basic APIs                                                        */
/**********************************************************************************************************/

void
thread_attr_init(pthread_attr_t *attr)
{
    int s;

    s = pthread_attr_init(attr);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_init");
    }
}

void
thread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
    int s;

    s = pthread_attr_setdetachstate(attr, detachstate);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_setdetachstate");
    }
}

void
thread_create(pthread_t *pthread_handle,
              pthread_attr_t *attr,
              void *(*start_routine)(void *),
              const void *thread_id)
{
    int s;

    s = pthread_create(pthread_handle, attr, start_routine, (void*)thread_id);
    if(0 != s)
    {
        handle_error_en(s, "pthread_create");
    }
}

void
thread_join(pthread_t thread_handler)
{
    int s = pthread_join(thread_handler, NULL);

    if(0 != s)
    {
        handle_error_en(s, "pthread_join");
    }
}

void
thread_mutex_init(pthread_mutex_t *mutex)
{
    int s = pthread_mutex_init(mutex, NULL);

    if(0 != s)
    {
        handle_error_en(s, "pthread_mutex_init");
    }
}

void
thread_mutex_lock(pthread_mutex_t *mutex)
{
    int s = pthread_mutex_lock(mutex);

    if(0 != s)
    {
        handle_error_en(s, "pthread_mutex_lock");
    }
}

void
thread_mutex_unlock(pthread_mutex_t *mutex)
{
    int s = pthread_mutex_unlock(mutex);

    if(0 != s)
    {
        handle_error_en(s, "pthread_mutex_unlock");
    }
}

void
thread_mutex_destroy(pthread_mutex_t *mutex)
{
    int s = pthread_mutex_destroy(mutex);

    if(0 != s)
    {
        handle_error_en(s, "pthread_mutex_destroy");
    }
}

void
thread_spin_init(pthread_spinlock_t *spinlock)
{
    int s = pthread_spin_init(spinlock, PTHREAD_PROCESS_PRIVATE);

    if(0 != s)
    {
        handle_error_en(s, "pthread_spin_init");
    }
}

void
thread_spin_lock(pthread_spinlock_t *spinlock)
{
    int s = pthread_spin_lock(spinlock);

    if(0 != s)
    {
        handle_error_en(s, "pthread_spin_lock");
    }
}

void
thread_spin_unlock(pthread_spinlock_t *spinlock)
{
    int s = pthread_spin_unlock(spinlock);

    if(0 != s)
    {
        handle_error_en(s, "pthread_spin_unlock");
    }
}

void
thread_spin_destroy(pthread_spinlock_t *spinlock)
{
    int s = pthread_spin_destroy(spinlock);

    if(0 != s)
    {
        handle_error_en(s, "pthread_spin_destroy");
    }
}

void
thread_cond_init(pthread_cond_t *cond)
{
    int s = pthread_cond_init(cond, NULL);

    if(0 != s)
    {
        handle_error_en(s, "pthread_cond_init");
    }
}

void
thread_cond_signal(pthread_cond_t *cond)
{
    int s = pthread_cond_signal(cond);

    if(0 != s)
    {
        handle_error_en(s, "pthread_cond_signal");
    }
}

void
thread_cond_broadcast(pthread_cond_t *cond)
{
    int s = pthread_cond_broadcast(cond);

    if(0 != s)
    {
        handle_error_en(s, "pthread_cond_broadcast");
    }
}

void
thread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex)
{
    int s = pthread_cond_wait(cond, mutex);

    if(0 != s)
    {
        handle_error_en(s, "pthread_cond_wait");
    }
}

void
thread_cond_destroy(pthread_cond_t *cond)
{
    int s = pthread_cond_destroy(cond);

    if(0 != s)
    {
        handle_error_en(s, "pthread_cond_destroy");
    }
}

void
thread_rwlock_init(pthread_rwlock_t *rw_lock)
{
    int s = pthread_rwlock_init(rw_lock, NULL);

    if(0 != s)
    {
        handle_error_en(s, "pthread_rwlock_init");
    }
}

void
thread_rwlock_rdlock(pthread_rwlock_t *rw_lock)
{
    int s = pthread_rwlock_rdlock(rw_lock);

    if(0 != s)
    {
        handle_error_en(s, "pthread_rwlock_rdlock");
    }
}

void
thread_rwlock_wrlock(pthread_rwlock_t *rw_lock)
{
    int s = pthread_rwlock_wrlock(rw_lock);

    if(0 != s)
    {
        handle_error_en(s, "pthread_rwlock_wrlock");
    }
}

void
thread_rwlock_unlock(pthread_rwlock_t *rw_lock)
{
    int s = pthread_rwlock_unlock(rw_lock);

    if(0 != s)
    {
        handle_error_en(s, "pthread_rwlock_unlock");
    }
}

void
thread_rwlock_destroy(pthread_rwlock_t *rw_lock)
{
    int s = pthread_rwlock_destroy(rw_lock);

    if(0 != s)
    {
        handle_error_en(s, "pthread_rwlock_destroy");
    }
}

/**********************************************************************************************************/
/*                                      Complex APIs                                                      */
/**********************************************************************************************************/

thread_t *
thread_init(thread_t *thread, char *name)
{
    if(!thread)
    {
        thread = calloc(1, sizeof(thread_t));
    }
    if(!thread)
    {
        handle_error("calloc");
    }

    strncpy(thread->name, name, sizeof(thread->name));
    thread->thread_created = false;
    thread->arg = NULL;
    thread->thread_fn = NULL;
    thread_attr_init(&thread->attributes);
    return thread;
}

void
thread_run(thread_t *thread, void *(*thread_fn)(void *), void *arg)
{
    thread->thread_fn = thread_fn;
    thread->arg = arg;
    thread->thread_created = true;
    thread_create(&thread->thread, &thread->attributes, thread_fn, arg);
}

void
thread_set_pause_fn(thread_t *thread, void *(*thread_pause_fn)(void *), void *pause_arg)
{
    thread->thread_pause_fn = thread_pause_fn;
    thread->pause_arg = pause_arg;
}

void
thread_pause(thread_t *thread)
{
    thread_mutex_lock(&thread->state_mutex);
    if(IS_BIT_SET(thread->flags, THREAD_F_RUNNING))
    {
        SET_BIT(thread->flags, THREAD_F_MARKED_FOR_PAUSE);
    }
    thread_mutex_unlock(&thread->state_mutex);
}

void
thread_resume(thread_t *thread)
{
    thread_mutex_lock(&thread->state_mutex);
    if(IS_BIT_SET(thread->flags, THREAD_F_PAUSED))
    {
        thread_cond_signal(&thread->cv);
    }
    thread_mutex_unlock(&thread->state_mutex);
}

void
thread_test_and_pause(thread_t *thread)
{
    thread_mutex_lock(&thread->state_mutex);
    if(IS_BIT_SET(thread->flags, THREAD_F_MARKED_FOR_PAUSE))
    {
        SET_BIT(thread->flags, THREAD_F_PAUSED);
        UNSET_BIT(thread->flags, THREAD_F_MARKED_FOR_PAUSE);
        UNSET_BIT(thread->flags, THREAD_F_RUNNING);
        thread_cond_wait(&thread->cv, &thread->state_mutex);
        UNSET_BIT(thread->flags, THREAD_F_PAUSED);
        SET_BIT(thread->flags, THREAD_F_RUNNING);
        (thread->thread_pause_fn)(thread->pause_arg);
    }
    thread_mutex_unlock(&thread->state_mutex);
}
