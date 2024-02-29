#include "threated_publisher.h"
#include "routing_table.h"
#include "utilities.h"
#include <pthread.h>
#include <stdio.h>

static routing_table_t publisher_routing_table;

static void
main_menu()
{
    int choice;

    while(1)
    {
        printf("Publisher Menu\n");
        printf("1. Add/Update routing table entry\n");
        printf("2. Delete routing table entry\n");
        printf("3. Dump routing table\n");
        printf("Enter Choice:");

        choice = 0;
        scanf("%d", &choice);

        switch(choice)
        {
            case 1:
                {
                    char dest[16];
                    int mask;
                    char oif[32];
                    char gw[16];

                    printf("Enter destination:");
                    scanf("%s", dest);
                    printf("Mask:");
                    scanf("%d", &mask);
                    printf("Enter oif name:");
                    scanf("%s", oif);
                    printf("Enter gateway IP:");
                    scanf("%s", gw);

                    routing_table_add_entry(&publisher_routing_table, dest, (char)mask, gw, oif);
                }
                break;
            case 2:
                {
                    char dest[16];
                    int mask;

                    printf("Enter destination:");
                    scanf("%s", dest);
                    printf("Mask:");
                    scanf("%d", &mask);

                    routing_table_delete_enty(&publisher_routing_table, dest, (char)mask);
                }
                break;
            case 3:
                routing_table_dump(&publisher_routing_table);
                break;
            default:
                break;
        }
    }
}

static void *
publisher_thread_fn(void *arg)
{
    routing_table_add_entry(&publisher_routing_table,
                            "122.1.1.1", 32, "10.1.1.1", "eth1");
    routing_table_add_entry(&publisher_routing_table,
                            "122.1.1.2", 32, "10.1.1.2", "eth1");
    routing_table_add_entry(&publisher_routing_table,
                            "122.1.1.3", 32, "10.1.1.3", "eth1");

    routing_table_dump(&publisher_routing_table);
    main_menu(&publisher_routing_table);

    return NULL;
}

void
publisher_create_thread(void)
{
    int s;

    pthread_attr_t attr;
    pthread_t publisher_thread;

    s = pthread_attr_init(&attr);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_init");
    }

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_setdetachstate");
    }

    s = pthread_create(&publisher_thread, &attr, publisher_thread_fn, (void*)0);
    if(0 != s)
    {
        handle_error_en(s, "pthread_create");
    }
}

void
publisher_init_routing_table(void)
{
    routing_table_init(&publisher_routing_table);
}

routing_table_t *
publisher_get_routing_table(void)
{
    return &publisher_routing_table;
}
