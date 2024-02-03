#include <stdio.h>
#include <stdbool.h>
#include "thread_barrier.h"
#include "threads.h"

void
thread_barrier_init(thread_barrier_t *barrier, unsigned int threshold_count)
{
    barrier->threshold_count = threshold_count;
    barrier->current_wait_count = 0;
    thread_cond_init(&barrier->cv);
    thread_mutex_init(&barrier->mutex);
    barrier->is_ready_again = true;
    thread_cond_init(&barrier->busy_cv);
}

void
thread_barrier_wait(thread_barrier_t *barrier)
{
    thread_mutex_lock(&barrier->mutex);

    while(!(barrier->is_ready_again))
    {
        thread_cond_wait(&barrier->busy_cv, &barrier->mutex);
    }

    if(barrier->current_wait_count + 1 == barrier->threshold_count)
    {
        barrier->is_ready_again = false;
        thread_cond_signal(&barrier->cv);
        thread_mutex_unlock(&barrier->mutex);
        return;
    }

    barrier->current_wait_count++;
    thread_cond_wait(&barrier->cv, &barrier->mutex);
    barrier->current_wait_count--;

    if(0 == barrier->current_wait_count)
    {
        barrier->is_ready_again = true;
        thread_cond_broadcast(&barrier->busy_cv);
    }
    else
    {
        thread_cond_signal(&barrier->cv);
    }

    thread_mutex_unlock(&barrier->mutex);
}

void
thread_barrier_destroy(thread_barrier_t *barrier)
{
    (void)barrier;
}

void
thread_barrier_print(thread_barrier_t *barrier)
{
    printf("thread_barrier->threshold_count = %u\n", barrier->threshold_count);
    printf("thread_barrier->current_wait_count = %u\n", barrier->current_wait_count);
    printf("thread_barrier->is_ready_again = %s\n", barrier->is_ready_again ? "true" : "false");
}
