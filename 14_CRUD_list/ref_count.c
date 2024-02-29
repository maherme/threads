#include "ref_count.h"
#include "threads.h"
#include <stdbool.h>
#include <assert.h>

void
ref_count_init(ref_count_t *ref_count)
{
    ref_count->ref_count = 0;
    thread_spin_init(&ref_count->spinlock);
}

void
ref_count_inc(ref_count_t *ref_count)
{
    thread_spin_lock(&ref_count->spinlock);
    ref_count->ref_count++;
    thread_spin_unlock(&ref_count->spinlock);
}

bool
ref_count_dec(ref_count_t *ref_count)
{
    bool is_ref_count_zero;
    thread_spin_lock(&ref_count->spinlock);
    assert(ref_count->ref_count);
    ref_count->ref_count--;
    is_ref_count_zero = (0 == ref_count->ref_count) ? true : false;
    thread_spin_unlock(&ref_count->spinlock);
    return is_ref_count_zero;
}

void
ref_count_destroy(ref_count_t *ref_count)
{
    assert(0 == ref_count->ref_count);
    thread_spin_destroy(&ref_count->spinlock);
}

void
thread_using_object(ref_count_t *ref_count)
{
    ref_count_inc(ref_count);
}

bool
thread_using_object_done(ref_count_t *ref_count)
{
    return ref_count_dec(ref_count);
}
