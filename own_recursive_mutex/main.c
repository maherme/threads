#include <stdlib.h>
#include <stdio.h>
#include "threads.h"
#include "recursive_mutex.h"

#define NUMBER_LOOPS    5

static rec_mutex_t rec_mutex;

static void *
fn_callback1(void *arg)
{
    (void)arg;

    for(int i = 1; i <= NUMBER_LOOPS; i++)
    {
        printf("Locking recursive mutex %d times\n", i);
        rec_mutex_lock(&rec_mutex);
    }

    printf("Executing critical section\n");

    for(int i = 1; i <= NUMBER_LOOPS; i++)
    {
        printf("Unlocking recursive mutex %d times\n", i);
        rec_mutex_unlock(&rec_mutex);
    }

    return NULL;
}

int main()
{
    pthread_t th1;

    rec_mutex_init(&rec_mutex);
    thread_create(&th1, fn_callback1, NULL);
    thread_join(th1);
    rec_mutex_destroy(&rec_mutex);

    printf("Program Finished\n");
    exit(EXIT_SUCCESS);
}
