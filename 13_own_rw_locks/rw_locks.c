#include "rw_locks.h"
#include "threads.h"
#include <stdbool.h>
#include <assert.h>

void
rw_lock_init(rw_lock_t *rw_lock)
{
    thread_mutex_init(&rw_lock->state_mutex);
    thread_cond_init(&rw_lock->cv);
    rw_lock->n_reader_waiting = 0;
    rw_lock->n_writer_waiting = 0;
    rw_lock->is_locked_by_reader = false;
    rw_lock->is_locked_by_writer = false;
    rw_lock->writer_thread = 0;
}

void
rw_lock_destroy(rw_lock_t *rw_lock)
{
    assert(0 == rw_lock->n_locks);
    assert(0 == rw_lock->n_reader_waiting);
    assert(0 == rw_lock->n_writer_waiting);
    assert(false == rw_lock->is_locked_by_writer);
    assert(false == rw_lock->is_locked_by_reader);
    assert(!rw_lock->writer_thread);
    thread_mutex_destroy(&rw_lock->state_mutex);
    thread_cond_destroy(&rw_lock->cv);
}

void
rw_lock_rd_lock(rw_lock_t *rw_lock)
{
    thread_mutex_lock(&rw_lock->state_mutex);

    /* rw_lock is available */
    if(!rw_lock->is_locked_by_reader && !rw_lock->is_locked_by_writer)
    {
        assert(0 == rw_lock->n_locks);
        assert(!rw_lock->writer_thread);
        rw_lock->n_locks++;
        rw_lock->is_locked_by_reader = true;
        thread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    /* rw_lock is locked by reader(s) */
    if(rw_lock->is_locked_by_reader)
    {
        assert(!rw_lock->is_locked_by_writer);
        assert(rw_lock->n_locks);
        assert(!rw_lock->writer_thread);
        rw_lock->n_locks++;
        thread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    /* rw_lock is locked by writer */
    while(rw_lock->is_locked_by_writer)
    {
        assert(rw_lock->n_locks == 1);
        assert(!rw_lock->is_locked_by_reader);
        assert(rw_lock->writer_thread);
        rw_lock->n_reader_waiting++;
        thread_cond_wait(&rw_lock->cv, &rw_lock->state_mutex);
        rw_lock->n_reader_waiting--;
    }
    if(0 == rw_lock->n_locks)
    {
        rw_lock->is_locked_by_reader = true;
    }
    rw_lock->n_locks++;
    assert(!rw_lock->is_locked_by_writer);
    assert(!rw_lock->writer_thread);
    thread_mutex_unlock(&rw_lock->state_mutex);
}

void
rw_lock_wr_lock(rw_lock_t *rw_lock)
{
    thread_mutex_lock(&rw_lock->state_mutex);

    /* rw_lock is available */
    if(!rw_lock->is_locked_by_reader && !rw_lock->is_locked_by_writer)
    {
        assert(0 == rw_lock->n_locks);
        assert(!rw_lock->is_locked_by_reader);
        assert(!rw_lock->is_locked_by_writer);
        assert(!rw_lock->writer_thread);
        rw_lock->n_locks++;
        rw_lock->is_locked_by_writer = true;
        rw_lock->writer_thread = pthread_self();
        thread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    /* rw_lock is locked by a same writer thread */
    if(rw_lock->is_locked_by_writer && pthread_self() == rw_lock->writer_thread)
    {
        assert(!rw_lock->is_locked_by_reader);
        assert(rw_lock->n_locks);
        rw_lock->n_locks++;
        thread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    /* rw_lock is already locked by any other thread */
    while(rw_lock->is_locked_by_reader || rw_lock->is_locked_by_writer)
    {
        if(rw_lock->is_locked_by_reader)
        {
            assert(!rw_lock->is_locked_by_writer);
            assert(rw_lock->n_locks);
            assert(!rw_lock->writer_thread);
        }
        else if(rw_lock->is_locked_by_writer)
        {
            assert(!rw_lock->is_locked_by_reader);
            assert(1 == rw_lock->n_locks);
            assert(rw_lock->writer_thread);
        }
        else { /* do nothing */}
        rw_lock->n_writer_waiting++;
        thread_cond_wait(&rw_lock->cv, &rw_lock->state_mutex);
        rw_lock->n_writer_waiting--;
    }
    assert(!rw_lock->is_locked_by_reader);
    assert(!rw_lock->is_locked_by_writer);
    assert(0 == rw_lock->n_locks);
    assert(!rw_lock->writer_thread);
    rw_lock->is_locked_by_writer = true;
    rw_lock->n_locks++;
    rw_lock->writer_thread = pthread_self();
    thread_mutex_unlock(&rw_lock->state_mutex);
}

void
rw_lock_unlock(rw_lock_t *rw_lock)
{
    thread_mutex_lock(&rw_lock->state_mutex);

    /* Attempt to unlock the unlocked lock */
    assert(rw_lock->n_locks);

    /* Writer thread unlocking the rw lock */
    if(rw_lock->is_locked_by_writer)
    {
        assert(rw_lock->writer_thread == pthread_self());
        assert(!rw_lock->is_locked_by_reader);
        rw_lock->n_locks--;
        if(rw_lock->n_locks)
        {
            thread_mutex_unlock(&rw_lock->state_mutex);
            return;
        }
        if(rw_lock->n_reader_waiting || rw_lock->n_writer_waiting)
        {
            thread_cond_broadcast(&rw_lock->cv);
        }
        rw_lock->is_locked_by_writer = false;
        rw_lock->writer_thread = 0;
        thread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    /* Reader thread unlocking the rw lock */
    if(rw_lock->is_locked_by_reader)
    {
        assert(!rw_lock->writer_thread);
        assert(!rw_lock->is_locked_by_writer);
        rw_lock->n_locks--;
        if(rw_lock->n_locks)
        {
            thread_mutex_unlock(&rw_lock->state_mutex);
            return;
        }
        if(rw_lock->n_reader_waiting || rw_lock->n_writer_waiting)
        {
            thread_cond_broadcast(&rw_lock->cv);
        }
        rw_lock->is_locked_by_reader = false;
        thread_mutex_unlock(&rw_lock->state_mutex);
        return;
    }

    assert(0);
}
