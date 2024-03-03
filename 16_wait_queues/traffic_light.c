#include "traffic_light.h"
#include "threadlib.h"
#include <stdbool.h>

void
traffic_light_init(traffic_light_t *traffic_light)
{
    direction_t direction = FIRST_DIRECTION;

    for(; direction < DIRECTION_MAX_NUM; direction++)
    {
        traffic_light->traffic_light_face[direction].light[RED] = false;
        traffic_light->traffic_light_face[direction].light[YELLOW] = false;
        traffic_light->traffic_light_face[direction].light[GREEN] = false;
        thread_mutex_init(&traffic_light->traffic_light_face[direction].mutex);
        wait_queue_init(&traffic_light->traffic_light_face[direction].wq);
    }
}

void
traffic_light_set_status(traffic_light_t *traffic_light,
                         direction_t direction,
                         traffic_light_color_t color,
                         bool status)
{
    traffic_light_color_t color_iterator = FIRST_COLOR;

    if(!status)
    {
        traffic_light->traffic_light_face[direction].light[color] = false;
        return;
    }

    for(; color_iterator < COLOR_MAX_NUM; color_iterator++)
    {
        if(color_iterator == color)
        {
            traffic_light->traffic_light_face[direction].light[color_iterator] = true;
            if(color == GREEN)
            {
                wait_queue_broadcast(&traffic_light->traffic_light_face[direction].wq, false);
            }
        }
        else
        {
            traffic_light->traffic_light_face[direction].light[color_iterator] = false;
        }
    }
}

