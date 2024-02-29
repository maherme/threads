#include "threated_subscriber.h"
#include "threated_publisher.h"
#include "routing_table.h"
#include "notification_chain.h"
#include "utilities.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

static void
test_callback(int client_id, void *arg, size_t arg_size, nfc_type_update_t nfc_op_code)
{
    routing_table_entry_t *routing_table_entry;

    printf("%s() client: %d, notified with operation code %s\n",
           __FUNCTION__, client_id, nfc_get_str_operation_code(nfc_op_code));

    routing_table_entry = (routing_table_entry_t*)arg;

    printf("%-20s %-4d %-20s %s\n", routing_table_entry->routing_table_entry_keys.dest,
                                    routing_table_entry->routing_table_entry_keys.mask,
                                    routing_table_entry->gw_ip,
                                    routing_table_entry->oif);
    printf("\n");
}

static void *
subscriber_thread_fn(void *arg)
{
    routing_table_entry_keys_t routing_table_entry_keys;
    int client_id = *(int*)arg;
    free(arg);

    memset(&routing_table_entry_keys, 0, sizeof(routing_table_entry_keys_t));
    strncpy(routing_table_entry_keys.dest, "122.1.1.1", sizeof(routing_table_entry_keys.dest));
    routing_table_entry_keys.mask = 32;
    routing_table_register_for_notification(publisher_get_routing_table(),
                                            &routing_table_entry_keys,
                                            sizeof(routing_table_entry_keys_t),
                                            test_callback,
                                            client_id);

    memset(&routing_table_entry_keys, 0, sizeof(routing_table_entry_keys_t));
    strncpy(routing_table_entry_keys.dest, "122.1.1.5", sizeof(routing_table_entry_keys.dest));
    routing_table_entry_keys.mask = 32;
    routing_table_register_for_notification(publisher_get_routing_table(),
                                            &routing_table_entry_keys,
                                            sizeof(routing_table_entry_keys_t),
                                            test_callback,
                                            client_id);

    memset(&routing_table_entry_keys, 0, sizeof(routing_table_entry_keys_t));
    strncpy(routing_table_entry_keys.dest, "122.1.1.6", sizeof(routing_table_entry_keys.dest));
    routing_table_entry_keys.mask = 32;
    routing_table_register_for_notification(publisher_get_routing_table(),
                                            &routing_table_entry_keys,
                                            sizeof(routing_table_entry_keys_t),
                                            test_callback,
                                            client_id);

    pause();
    return NULL;
}

void
create_subscriber_thread(int client_id)
{
    int s;

    pthread_attr_t attr;
    pthread_t subscriber_thread;
    int *subscriber_id;

    subscriber_id = calloc(1, sizeof(int));
    if(NULL == subscriber_id)
    {
        handle_error("calloc");
    }
    *subscriber_id = client_id;

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

    s = pthread_create(&subscriber_thread, &attr, subscriber_thread_fn, (void*)subscriber_id);
    if(0 != s)
    {
        handle_error_en(s, "pthread_create");
    }

    printf("Subscriber %d created\n", client_id);
}
