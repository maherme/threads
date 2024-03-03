#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <pthread.h>
#include <stdbool.h>
#include "threadlib.h"

typedef enum
{
    RED,
    FIRST_COLOR = RED,
    YELLOW,
    GREEN,
    COLOR_MAX_NUM
}traffic_light_color_t;

typedef enum
{
    EAST,
    FIRST_DIRECTION = EAST,
    WEST,
    NORTH,
    SOUTH,
    DIRECTION_MAX_NUM
}direction_t;

typedef struct
{
    bool light[COLOR_MAX_NUM];  /**< @brief state of the traffic light, only one element can be true */
    pthread_mutex_t mutex;      /**< @brief for changing the traffic light status in mutual exclusive way */
    wait_queue_t wq;
}traffic_light_face_t;

typedef struct
{
    traffic_light_face_t traffic_light_face[DIRECTION_MAX_NUM];
}traffic_light_t;

typedef enum
{
    THREAD_RUN_SLOW,    /**< @brief on yellow light */
    THREAD_RUN_NORMAL,  /**< @brief on green light */
    THREAD_STOP         /**< @brief on red light */
}thread_state_t;

typedef struct
{
    direction_t direction;
    thread_state_t thread_state;
    thread_t *thread;
    traffic_light_t *traffic_light;
}thread_private_data_t;

void
traffic_light_init(traffic_light_t *traffic_light);

void
traffic_light_set_status(traffic_light_t *traffic_light,
                         direction_t direction,
                         traffic_light_color_t color,
                         bool status);

#endif /* TRAFFIC_LIGHT_H */
