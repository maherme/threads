#include <pthread.h>
#include <assert.h>
#include "recursive_mutex.h"
#include "threads.h"

void
rec_mutex_init(rec_mutex_t *rec_mutex)
{
    rec_mutex->n = 0;
    rec_mutex->locking_thread = 0;
    thread_cond_init(&rec_mutex->cv);
    thread_mutex_init(&rec_mutex->mutex);
    rec_mutex->threads_waiting = 0;
}

void
rec_mutex_lock(rec_mutex_t *rec_mutex)
{
    thread_mutex_lock(&rec_mutex->mutex);

    /* Checking recursive mutex object is not already locked */
    if(!rec_mutex->n)
    {
        assert(!rec_mutex->locking_thread);
        rec_mutex->n = 1;
        rec_mutex->locking_thread = pthread_self();
        thread_mutex_unlock(&rec_mutex->mutex);
        return;
    }

    /* Checking recursive mutex is locked by self already */
    if(pthread_self() == rec_mutex->locking_thread)
    {
        assert(rec_mutex->n);
        rec_mutex->n++;
        thread_mutex_unlock(&rec_mutex->mutex);
        return;
    }

    /* Checking recursive mutex is locked by other thread */
    while(rec_mutex->locking_thread && (pthread_self() != rec_mutex->locking_thread))
    {
        rec_mutex->threads_waiting++;
        thread_cond_wait(&rec_mutex->cv, &rec_mutex->mutex);
        rec_mutex->threads_waiting--;
    }

    rec_mutex->n = 1;
    rec_mutex->locking_thread = pthread_self();
    thread_mutex_unlock(&rec_mutex->mutex);
}

void
rec_mutex_unlock(rec_mutex_t *rec_mutex)
{
    thread_mutex_lock(&rec_mutex->mutex);

    /* Checking recursive mutex object is not already locked */
    if(!rec_mutex->n)
    {
        assert(!rec_mutex->n);
        assert(0);
    }

    /* Checking recursive mutex is locked by self already */
    if(pthread_self() == rec_mutex->locking_thread)
    {
        assert(rec_mutex->n);
        rec_mutex->n--;

        if(0 < rec_mutex->n)
        {
            thread_mutex_unlock(&rec_mutex->mutex);
            return;
        }

        if(rec_mutex->threads_waiting)
        {
            thread_cond_signal(&rec_mutex->cv);
        }

        rec_mutex->locking_thread = 0;
        thread_mutex_unlock(&rec_mutex->mutex);
        return;
    }

    /* Checking recursive mutex is locked by other thread */
    while(rec_mutex->locking_thread && (pthread_self() != rec_mutex->locking_thread))
    {
        assert(0);
    }
}

void
rec_mutex_destroy(rec_mutex_t *rec_mutex)
{
    assert(!rec_mutex->n);
    assert(!rec_mutex->locking_thread);
    assert(!rec_mutex->threads_waiting);
    thread_mutex_destroy(&rec_mutex->mutex);
    thread_cond_destroy(&rec_mutex->cv);
}
