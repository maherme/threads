#include "routing_table.h"
#include "threated_publisher.h"
#include "threated_subscriber.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    publisher_init_routing_table();

    create_subscriber_thread(1);
    sleep(1);

    create_subscriber_thread(2);
    sleep(1);

    create_subscriber_thread(3);
    sleep(1);

    publisher_create_thread();
    printf("Publisher thread created\n");

    pthread_exit(NULL);
}
