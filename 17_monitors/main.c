#include "monitor_rw.h"
#include "threadlib.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

static monitor_rw_t monitor_rw;
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
    else if(0 < n_r && n_r <= monitor_rw.n_max_readers && 0 == n_w)
    {
        /* valid condition */
    }
    else if(0 == n_r && n_w <= monitor_rw.n_max_writers && 0 < n_w)
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
        monitor_rw_rd_lock(&monitor_rw);
        reader_enter_cs();
        execute_cs();
        reader_exit_cs();
        monitor_rw_unlock(&monitor_rw);
    }

    return NULL;
}

static void
writer_enter_cs(void)
{
    thread_mutex_lock(&state_check_mutex);
    n_w++;
    thread_mutex_unlock(&state_check_mutex);
}

static void
writer_exit_cs(void)
{
    thread_mutex_lock(&state_check_mutex);
    n_w--;
    thread_mutex_unlock(&state_check_mutex);
}

static void *
write_thread_fn(void *arg)
{
    (void)arg;

    while(1)
    {
        monitor_rw_wr_lock(&monitor_rw);
        writer_enter_cs();
        execute_cs();
        writer_exit_cs();
        monitor_rw_unlock(&monitor_rw);
    }

    return NULL;
}

int
main()
{
    static pthread_t th1, th2, th3, th4, th5, th6;

    pthread_attr_t attr;
    thread_attr_init(&attr);
    thread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    monitor_rw_init(&monitor_rw);
    monitor_rw_set_max_readers_writers(&monitor_rw, 3, 3);
    thread_mutex_init(&state_check_mutex);

    thread_create(&th1, &attr, read_thread_fn, NULL);
    thread_create(&th2, &attr, read_thread_fn, NULL);
    thread_create(&th3, &attr, read_thread_fn, NULL);
    thread_create(&th4, &attr, write_thread_fn, NULL);
    thread_create(&th5, &attr, write_thread_fn, NULL);
    thread_create(&th6, &attr, write_thread_fn, NULL);
    pthread_exit(0);
    exit(EXIT_SUCCESS);
}
