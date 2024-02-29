#include "rw_locks.h"
#include "threads.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static rw_lock_t rw_lock;
static int n_r = 0;
static int n_w = 0;
pthread_mutex_t state_check_mutex;

static void
execute_cs(void)
{
    thread_mutex_lock(&state_check_mutex);
    assert(0 <= n_r && 0 <= n_w);
    if(0 == n_r && 0 == n_w)
    {
        /* valid condition */
    }
    else if(0 < n_r && 0 == n_w)
    {
        /* valid condition */
    }
    else if(0 == n_r && 1 == n_w)
    {
        /* valid condition */
    }
    else
    {
        assert(0);
    }
    printf("n_r = %d, n_w = %d\n", n_r, n_w);
    thread_mutex_unlock(&state_check_mutex);
}

static void
reader_enter_cs(void)
{
    thread_mutex_lock(&state_check_mutex);
    n_r++;
    thread_mutex_unlock(&state_check_mutex);
}

static void
reader_exit_cs(void)
{
    thread_mutex_lock(&state_check_mutex);
    n_r--;
    thread_mutex_unlock(&state_check_mutex);
}

static void *
read_thread_fn(void *arg)
{
    (void)arg;

    while(1)
    {
        rw_lock_rd_lock(&rw_lock);
        reader_enter_cs();
        execute_cs();
        reader_exit_cs();
        rw_lock_unlock(&rw_lock);
    }

    return NULL;
}

static void
writer_enter_cs(void)
{
    n_w++;
}

static void
writer_exit_cs(void)
{
    n_w--;
}

static void *
write_thread_fn(void *arg)
{
    (void)arg;

    while(1)
    {
        rw_lock_wr_lock(&rw_lock);
        writer_enter_cs();
        execute_cs();
        writer_exit_cs();
        rw_lock_unlock(&rw_lock);
    }

    return NULL;
}

int
main()
{
    static pthread_t th1, th2, th3, th4, th5, th6;
    rw_lock_init(&rw_lock);
    thread_mutex_init(&state_check_mutex);
    thread_create(&th1, read_thread_fn, NULL);
    thread_create(&th2, read_thread_fn, NULL);
    thread_create(&th3, read_thread_fn, NULL);
    thread_create(&th4, write_thread_fn, NULL);
    thread_create(&th5, write_thread_fn, NULL);
    thread_create(&th6, write_thread_fn, NULL);
    pthread_exit(0);
    exit(EXIT_SUCCESS);
}
