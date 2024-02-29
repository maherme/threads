#ifndef DINNER_PHY_H
#define DINNER_PHY_H

#include <pthread.h>
#include <stdbool.h>

typedef struct
{
    int phil_id;
    pthread_t thread_handle;
    int eat_count;
}philosopher_t;

typedef struct
{
    int spoon_id;
    bool is_used;
    philosopher_t *phil;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
}spoon_t;

void
spoons_init(void);

void
philosophers_init(void);

void
philosophers_create(void);

#endif /* DINNER_PHY_H */
