#include "monitor_rw.h"
#include "threadlib.h"
#include <stdbool.h>
#include <assert.h>

void
monitor_rw_init(monitor_rw_t *monitor_rw)
{
    thread_mutex_init(&monitor_rw->state_mutex);
    thread_cond_init(&monitor_rw->cv_reader);
    thread_cond_init(&monitor_rw->cv_writer);
    monitor_rw->dont_allow_readers = false;
    monitor_rw->dont_allow_writers = false;
    monitor_rw->n_reader_waiting = 0;
    monitor_rw->n_writer_waiting = 0;
    monitor_rw->is_locked_by_reader = false;
    monitor_rw->is_locked_by_writer = false;
    monitor_rw->n_max_readers = MONITOR_RW_MAX_READER_THREADS_DEF;
    monitor_rw->n_max_writers = MONITOR_RW_MAX_WRITER_THREADS_DEF;
}

void
monitor_rw_set_max_readers_writers(monitor_rw_t *monitor_rw, uint16_t max_readers, uint16_t max_writers)
{
    monitor_rw->n_max_readers = max_readers;
    monitor_rw->n_max_writers = max_writers;
}

void
monitor_rw_destroy(monitor_rw_t *monitor_rw)
{
    assert(0 == monitor_rw->n_locks);
    assert(0 == monitor_rw->n_reader_waiting);
    assert(0 == monitor_rw->n_writer_waiting);
    assert(!monitor_rw->is_locked_by_writer);
    assert(!monitor_rw->is_locked_by_reader);
    thread_mutex_destroy(&monitor_rw->state_mutex);
    thread_cond_destroy(&monitor_rw->cv_reader);
    thread_cond_destroy(&monitor_rw->cv_writer);
}

void
monitor_rw_rd_lock(monitor_rw_t *monitor_rw)
{
    thread_mutex_lock(&monitor_rw->state_mutex);

    /* monitor_rw is available */
    if(!monitor_rw->is_locked_by_reader &&
       !monitor_rw->is_locked_by_writer &&
       1 <= monitor_rw->n_max_readers &&
       !monitor_rw->dont_allow_readers)
    {
        assert(0 == monitor_rw->n_locks);
        monitor_rw->n_locks++;
        monitor_rw->is_locked_by_reader = true;
        thread_mutex_unlock(&monitor_rw->state_mutex);
        return;
    }

    /* monitor_rw is locked by writer */
    while(monitor_rw->is_locked_by_writer ||
          monitor_rw->n_locks == monitor_rw->n_max_readers ||
          monitor_rw->dont_allow_readers)
    {
        monitor_rw->n_reader_waiting++;
        thread_cond_wait(&monitor_rw->cv_reader, &monitor_rw->state_mutex);
        monitor_rw->n_reader_waiting--;
    }
    if(0 == monitor_rw->n_locks)
    {
        monitor_rw->is_locked_by_reader = true;
    }
    monitor_rw->n_locks++;
    assert(!monitor_rw->is_locked_by_writer);
    thread_mutex_unlock(&monitor_rw->state_mutex);
}

void
monitor_rw_wr_lock(monitor_rw_t *monitor_rw)
{
    thread_mutex_lock(&monitor_rw->state_mutex);

    /* monitor_rw is available */
    if(!monitor_rw->is_locked_by_reader &&
       !monitor_rw->is_locked_by_writer &&
       1 <= monitor_rw->n_max_writers &&
       !monitor_rw->dont_allow_writers)
    {
        assert(0 == monitor_rw->n_locks);
        monitor_rw->n_locks++;
        monitor_rw->is_locked_by_writer = true;
        thread_mutex_unlock(&monitor_rw->state_mutex);
        return;
    }

    /* monitor_rw is locked by reader */
    while(monitor_rw->is_locked_by_reader ||
          monitor_rw->n_locks == monitor_rw->n_max_writers ||
          monitor_rw->dont_allow_writers)
    {
        monitor_rw->n_writer_waiting++;
        thread_cond_wait(&monitor_rw->cv_writer, &monitor_rw->state_mutex);
        monitor_rw->n_writer_waiting--;
    }
    if(0 == monitor_rw->n_locks)
    {
        monitor_rw->is_locked_by_writer = true;
    }
    monitor_rw->n_locks++;
    assert(!monitor_rw->is_locked_by_reader);
    thread_mutex_unlock(&monitor_rw->state_mutex);
}

void
monitor_rw_unlock(monitor_rw_t *monitor_rw)
{
    thread_mutex_lock(&monitor_rw->state_mutex);

    /* Attempt to unlock the unlocked lock */
    assert(monitor_rw->n_locks);

    /* Writer thread unlocking the rw lock */
    if(monitor_rw->is_locked_by_writer)
    {
        assert(!monitor_rw->is_locked_by_reader);
        monitor_rw->n_locks--;
        if(monitor_rw->n_locks)
        {
            if(monitor_rw->n_writer_waiting)
            {
                thread_cond_signal(&monitor_rw->cv_writer);
            }
            thread_mutex_unlock(&monitor_rw->state_mutex);
            return;
        }
        if(monitor_rw->n_reader_waiting)
        {
            thread_cond_broadcast(&monitor_rw->cv_reader);
            monitor_rw->dont_allow_writers = true;
        }
        else if(monitor_rw->n_writer_waiting)
        {
            thread_cond_broadcast(&monitor_rw->cv_writer);
        }
        else { /* do  noghing */ }
        monitor_rw->dont_allow_readers = false;
        monitor_rw->is_locked_by_writer = false;
        thread_mutex_unlock(&monitor_rw->state_mutex);
        return;
    }

    /* Reader thread unlocking the rw lock */
    if(monitor_rw->is_locked_by_reader)
    {
        assert(!monitor_rw->is_locked_by_writer);
        monitor_rw->n_locks--;
        if(monitor_rw->n_locks)
        {
            if(monitor_rw->n_reader_waiting)
            {
                thread_cond_signal(&monitor_rw->cv_reader);
            }
            thread_mutex_unlock(&monitor_rw->state_mutex);
            return;
        }
        if(monitor_rw->n_writer_waiting)
        {
            thread_cond_broadcast(&monitor_rw->cv_writer);
            monitor_rw->dont_allow_readers = true;
        }
        else if(monitor_rw->n_reader_waiting)
        {
            thread_cond_broadcast(&monitor_rw->cv_reader);
        }
        else {/* do nothing */}
        monitor_rw->dont_allow_writers = false;
        monitor_rw->is_locked_by_reader = false;
        thread_mutex_unlock(&monitor_rw->state_mutex);
        return;
    }

    assert(0);
}
