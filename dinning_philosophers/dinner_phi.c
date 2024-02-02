#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>
#include "dinner_phi.h"
#include "threads.h"

#define NUMBER_PHILOSOPHERS     5

static philosopher_t phils[NUMBER_PHILOSOPHERS];
static spoon_t spoons[NUMBER_PHILOSOPHERS];

static spoon_t *
phil_get_right_spoon(philosopher_t *phil)
{
    int phil_id = phil->phil_id;

    if(!phil_id)
    {
        return &spoons[NUMBER_PHILOSOPHERS - 1];
    }

    return &spoons[phil_id - 1];
}

static spoon_t *
phil_get_left_spoon(philosopher_t *phil)
{
    return &spoons[phil->phil_id];
}

static void
phil_eat(philosopher_t *phil)
{
    spoon_t *left_spoon = phil_get_left_spoon(phil);
    spoon_t *right_spoon = phil_get_right_spoon(phil);

    assert(phil == left_spoon->phil);
    assert(phil == right_spoon->phil);
    assert(true == left_spoon->is_used);
    assert(true == right_spoon->is_used);
    phil->eat_count++;
    printf("Philosopher %d eats with spoon [%d, %d] for %d times\n",
           phil->phil_id, left_spoon->spoon_id, right_spoon->spoon_id, phil->eat_count);
    sleep(1);
}

static void
phi_locks_spoon(philosopher_t *phil, spoon_t *spoon)
{
    printf("Philosopher %d waiting to lock spoon %d\n", phil->phil_id, spoon->spoon_id);
    thread_mutex_lock(&spoon->mutex);
    printf("Philosopher %d locks spoon %d\n", phil->phil_id, spoon->spoon_id);
}

static void
phi_grab_spoon(philosopher_t *phil, spoon_t *spoon)
{
    printf("Philosopher %d finds spoon %d available\n", phil->phil_id, spoon->spoon_id);
    spoon->is_used = true;
    spoon->phil = phil;
    thread_mutex_unlock(&spoon->mutex);
}

static void
phi_drop_spoon(philosopher_t *phil, spoon_t *spoon)
{
    assert(true == spoon->is_used);
    assert(phil == spoon->phil);
    spoon->is_used = false;
    spoon->phil = NULL;
    printf("Philosopher %d release spoon %d\n", phil->phil_id, spoon->spoon_id);
}

static void
phi_wait_spoon(philosopher_t *phil, spoon_t *spoon)
{
    phi_locks_spoon(phil, spoon);

    while(spoon->is_used && (phil != spoon->phil))
    {
        printf("Philosopher %d blocks himself, spoon %d is already in use\n",
               phil->phil_id, spoon->spoon_id);
        thread_cond_wait(&spoon->cv, &spoon->mutex);
        printf("Philosopher %d wakes up, checking the spoon %d again\n",
               phil->phil_id, spoon->spoon_id);
    }
}

static bool
phi_check_spoon_available(philosopher_t *phil, spoon_t *spoon)
{
    phi_locks_spoon(phil, spoon);

    if(!spoon->is_used)
    {
        return true;
    }

    return false;
}

static void
phil_release_both_spoons(philosopher_t *phil)
{
    spoon_t *left_spoon = phil_get_left_spoon(phil);
    spoon_t *right_spoon = phil_get_right_spoon(phil);

    phi_locks_spoon(phil, left_spoon);
    phi_locks_spoon(phil, right_spoon);

    phi_drop_spoon(phil, left_spoon);
    printf("Philosopher %d signalling and releasing spoon %d\n", phil->phil_id, left_spoon->spoon_id);
    thread_cond_signal(&left_spoon->cv);
    thread_mutex_unlock(&left_spoon->mutex);

    phi_drop_spoon(phil, right_spoon);
    printf("Philosopher %d signalling and releasing spoon %d\n", phil->phil_id, right_spoon->spoon_id);
    thread_cond_signal(&right_spoon->cv);
    thread_mutex_unlock(&right_spoon->mutex);
}

static bool
phil_release_spoons(philosopher_t *phil, spoon_t *busy_spoon, spoon_t *owned_spoon)
{
    if(phil != busy_spoon->phil)
    {
        printf("Philosopher %d finds spoon %d is already used by philosopher %d, releasing spoon %d\n",
               phil->phil_id, busy_spoon->spoon_id, busy_spoon->phil->phil_id, owned_spoon->spoon_id);
        thread_mutex_lock(&owned_spoon->mutex);
        phi_drop_spoon(phil, owned_spoon);
        thread_mutex_unlock(&owned_spoon->mutex);
        thread_mutex_unlock(&busy_spoon->mutex);
        return false;
    }
    else
    {
        printf("Philosopher %d already has spoon %d\n", phil->phil_id, busy_spoon->spoon_id);
        thread_mutex_unlock(&busy_spoon->mutex);
        return true;
    }
}

static bool
phil_get_access_both_spoons(philosopher_t *phil)
{
    spoon_t *left_spoon = phil_get_left_spoon(phil);
    spoon_t *right_spoon = phil_get_right_spoon(phil);

    phi_wait_spoon(phil, left_spoon);
    phi_grab_spoon(phil, left_spoon);

    if(phi_check_spoon_available(phil, right_spoon))
    {
        phi_grab_spoon(phil, right_spoon);
        return true;
    }
    else
    {
        if(phil_release_spoons(phil, right_spoon, left_spoon))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /* This code should never be executed */
    assert(0);
}

static void *
philosopher_fn(void *arg)
{
    philosopher_t *phil = (philosopher_t*)arg;

    while(true)
    {
        if(phil_get_access_both_spoons(phil))
        {
            phil_eat(phil);
            phil_release_both_spoons(phil);
            sleep(1);
        }
    }

    return NULL;
}

void
spoons_init(void)
{
    for(int i = 0; i < NUMBER_PHILOSOPHERS; i++)
    {
        spoons[i].spoon_id = i;
        spoons[i].is_used = false;
        spoons[i].phil = NULL;
        thread_mutex_init(&spoons[i].mutex);
        thread_cond_init(&spoons[i].cv);
    }
}

void
philosophers_init(void)
{
    for(int i = 0; i < NUMBER_PHILOSOPHERS; i++)
    {
        phils[i].phil_id = i;
        phils[i].eat_count = 0;
    }
}

void
philosophers_create(void)
{
    for(int i = 0; i < NUMBER_PHILOSOPHERS; i++)
    {
        thread_create(&phils[i].thread_handle, philosopher_fn, &phils[i]);
    }
}
