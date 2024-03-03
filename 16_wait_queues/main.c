#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "traffic_light.h"
#include "threadlib.h"

void
user_menu(traffic_light_t *traffic_light)
{
    int choice;
    direction_t direction = FIRST_DIRECTION;
    traffic_light_color_t color = FIRST_COLOR;

    while(1)
    {
        printf("Traffic light operation:\n");
        printf("1. East : Red\n");
        printf("2. East : Yellow\n");
        printf("3. East : Green\n");
        printf("4. West : Red\n");
        printf("5. West : Yellow\n");
        printf("6. West : Green\n");
        printf("7. North : Red\n");
        printf("8. North : Yellow\n");
        printf("9. North : Green\n");
        printf("10. South : Red\n");
        printf("11. South : Yellow\n");
        printf("12. South : Green\n");
        printf("Enter choice: ");

        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                direction = EAST;
                color = RED;
                break;
            case 2:
                direction = EAST;
                color = YELLOW;
                break;
            case 3:
                direction = EAST;
                color = GREEN;
                break;
            case 4:
                direction = WEST;
                color = RED;
                break;
            case 5:
                direction = WEST;
                color = YELLOW;
                break;
            case 6:
                direction = WEST;
                color = GREEN;
                break;
            case 7:
                direction = NORTH;
                color = RED;
                break;
            case 8:
                direction = NORTH;
                color = YELLOW;
                break;
            case 9:
                direction = NORTH;
                color = GREEN;
                break;
            case 10:
                direction = SOUTH;
                color = RED;
                break;
            case 11:
                direction = SOUTH;
                color = YELLOW;
                break;
            case 12:
                direction = SOUTH;
                color = GREEN;
                break;
            default:
                break;
        }
        thread_mutex_lock(&traffic_light->traffic_light_face[direction].mutex);
        traffic_light_set_status(traffic_light, direction, color, true);
        thread_mutex_unlock(&traffic_light->traffic_light_face[direction].mutex);
    }
}

static void
file_write(char *buffer)
{
    static FILE *fptr = NULL;

    if(!fptr)
    {
        fptr = fopen("log.txt", "w");
    }

    fwrite(buffer, sizeof(char), strlen(buffer), fptr);
    fflush(fptr);
}

static char log_buffer[256];

static bool
thread_block_on_traffic_light(void *arg, pthread_mutex_t **mutex)
{
    thread_private_data_t *private_data = (thread_private_data_t *)arg;

    if(mutex)
    {
        *mutex = &private_data->traffic_light->traffic_light_face->mutex;
        thread_mutex_lock(*mutex);
    }

    if(private_data->traffic_light->traffic_light_face[private_data->direction].light[RED])
    {
        return true;
    }

    return false;
}

static void *
thread_move(void *arg)
{
    thread_private_data_t *private_data = (thread_private_data_t *)arg;
    thread_t *thread = private_data->thread;
    traffic_light_t *traffic_light = private_data->traffic_light;
    direction_t direction = private_data->direction;

    while(1)
    {
        wait_queue_test_and_wait(&(traffic_light->traffic_light_face[direction].wq),
                                 thread_block_on_traffic_light,
                                 private_data);
        sprintf(log_buffer, "Thread %s running\n", thread->name);
        file_write(log_buffer);
        /* Critical section exit */
        thread_mutex_unlock(traffic_light->traffic_light_face[direction].wq.app_mutex);
        sleep(2);
    }

    return NULL;
}

int main()
{
    thread_t *thread;
    traffic_light_t *traffic_light = calloc(1, sizeof(traffic_light_t));
    traffic_light_init(traffic_light);

    thread = thread_init(NULL, "TH_EAST1");
    thread_private_data_t *thread_private_data_east1 = calloc(1, sizeof(thread_private_data_t));
    thread_private_data_east1->direction = EAST;
    thread_private_data_east1->thread_state = THREAD_RUN_NORMAL;
    thread_private_data_east1->thread = thread;
    thread_private_data_east1->traffic_light = traffic_light;
    thread_run(thread, thread_move, (void *)thread_private_data_east1);

    thread = thread_init(NULL, "TH_EAST2");
    thread_private_data_t *thread_private_data_east2 = calloc(1, sizeof(thread_private_data_t));
    thread_private_data_east2->direction = EAST;
    thread_private_data_east2->thread_state = THREAD_RUN_NORMAL;
    thread_private_data_east2->thread = thread;
    thread_private_data_east2->traffic_light = traffic_light;
    thread_run(thread, thread_move, (void *)thread_private_data_east2);

    thread = thread_init(NULL, "TH_WEST1");
    thread_private_data_t *thread_private_data_west1 = calloc(1, sizeof(thread_private_data_t));
    thread_private_data_west1->direction = WEST;
    thread_private_data_west1->thread_state = THREAD_RUN_NORMAL;
    thread_private_data_west1->thread = thread;
    thread_private_data_west1->traffic_light = traffic_light;
    thread_run(thread, thread_move, (void *)thread_private_data_west1);

    thread = thread_init(NULL, "TH_WEST2");
    thread_private_data_t *thread_private_data_west2 = calloc(1, sizeof(thread_private_data_t));
    thread_private_data_west2->direction = WEST;
    thread_private_data_west2->thread_state = THREAD_RUN_NORMAL;
    thread_private_data_west2->thread = thread;
    thread_private_data_west2->traffic_light = traffic_light;
    thread_run(thread, thread_move, (void *)thread_private_data_west2);

    user_menu(traffic_light);

    pthread_exit(0);
    exit(EXIT_SUCCESS);
}
