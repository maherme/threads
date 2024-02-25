#ifndef REF_COUNT_H
#define REF_COUNT_H

#include <stdbool.h>
#include <pthread.h>

typedef struct
{
    int ref_count;
    pthread_spinlock_t spinlock;
}ref_count_t;

void
ref_count_init(ref_count_t *ref_count);

void
ref_count_inc(ref_count_t *ref_count);

bool
ref_count_dec(ref_count_t *ref_count);

void
ref_count_destroy(ref_count_t *ref_count);

void
thread_using_object(ref_count_t *ref_count);

bool
thread_using_object_done(ref_count_t *ref_count);

#endif /* REF_COUNT_H */
