#include <pthread.h>
#include <assert.h>
#include <stdio.h>
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
phil_release_both_spoons(philosopher_t *phil)
{
    spoon_t *left_spoon = phil_get_left_spoon(phil);
    spoon_t *right_spoon = phil_get_right_spoon(phil);

    thread_mutex_lock(&left_spoon->mutex);
    thread_mutex_lock(&right_spoon->mutex);

    assert(phil == left_spoon->phil);
    assert(true == left_spoon->is_used);
    assert(phil == right_spoon->phil);
    assert(true == right_spoon->is_used);

    printf("Philosopher %d signalling and releasing spoon %d\n", phil->phil_id, left_spoon->spoon_id);
    left_spoon->is_used = false;
    left_spoon->phil = NULL;
    thread_cond_signal(&left_spoon->cv);
    thread_mutex_unlock(&left_spoon->mutex);

    printf("Philosopher %d signalling and releasing spoon %d\n", phil->phil_id, right_spoon->spoon_id);
    right_spoon->is_used = false;
    right_spoon->phil = NULL;
    thread_cond_signal(&right_spoon->cv);
    thread_mutex_unlock(&right_spoon->mutex);
}

static bool
phil_get_access_both_spoons(philosopher_t *phil)
{
    spoon_t *left_spoon = phil_get_left_spoon(phil);
    spoon_t *right_spoon = phil_get_right_spoon(phil);


    printf("Philosopher %d waiting to lock spoon %d\n", phil->phil_id, left_spoon->spoon_id);
    thread_mutex_lock(&left_spoon->mutex);
    printf("Philosopher %d locks spoon %d\n", phil->phil_id, left_spoon->spoon_id);

    while(left_spoon->is_used && (phil != left_spoon->phil))
    {
        printf("Philosopher %d blocks himself, spoon %d is already in use\n",
               phil->phil_id, left_spoon->spoon_id);
        thread_cond_wait(&left_spoon->cv, &left_spoon->mutex);
        printf("Philosopher %d wakes up, checking the spoon %d again\n",
               phil->phil_id, left_spoon->spoon_id);
    }

    printf("Philosopher %d finds spoon %d available\n", phil->phil_id, left_spoon->spoon_id);
    left_spoon->is_used = true;
    left_spoon->phil = phil;
    thread_mutex_unlock(&left_spoon->mutex);

    printf("Philosopher %d waiting to lock spoon %d\n", phil->phil_id, right_spoon->spoon_id);
    thread_mutex_lock(&right_spoon->mutex);
    printf("Philosopher %d locks spoon %d\n", phil->phil_id, right_spoon->spoon_id);

    if(!right_spoon->is_used)
    {
        right_spoon->is_used = true;
        right_spoon->phil = phil;
        thread_mutex_unlock(&right_spoon->mutex);
        return true;
    }

    if(phil != right_spoon->phil)
    {
        printf("Philosopher %d finds spoon %d is already used by philosopher %d, releasing spoon %d\n",
               phil->phil_id, right_spoon->spoon_id, right_spoon->phil->phil_id, left_spoon->spoon_id);
        thread_mutex_lock(&left_spoon->mutex);
        assert(true == left_spoon->is_used);
        assert(phil == left_spoon->phil);
        left_spoon->is_used = false;
        left_spoon->phil = NULL;
        printf("Philosopher %d release spoon %d\n", phil->phil_id, left_spoon->spoon_id);
        thread_mutex_unlock(&left_spoon->mutex);
        thread_mutex_unlock(&right_spoon->mutex);
        return false;
    }
    else
    {
        printf("Philosopher %d already has spoon %d\n", phil->phil_id, right_spoon->spoon_id);
        thread_mutex_unlock(&right_spoon->mutex);
        return true;
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

int
main()
{
    for(int i = 0; i < NUMBER_PHILOSOPHERS; i++)
    {
        spoons[i].spoon_id = i;
        spoons[i].is_used = false;
        spoons[i].phil = NULL;
        thread_mutex_init(&spoons[i].mutex);
        thread_cond_init(&spoons[i].cv);
    }

    for(int i = 0; i < NUMBER_PHILOSOPHERS; i++)
    {
        phils[i].phil_id = i;
        phils[i].eat_count = 0;
        thread_create(&phils[i].thread_handle, philosopher_fn, &phils[i]);
    }

    pthread_exit(NULL);
}
