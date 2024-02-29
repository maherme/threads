#ifndef ROUTING_TABLE_H
#define ROUTING_TABLE_H

#include "notification_chain.h"
#include <stdbool.h>

typedef struct
{
    char dest[16];
    char mask;
} routing_table_entry_keys_t;

typedef struct _routing_table_entry
{
    routing_table_entry_keys_t routing_table_entry_keys;
    char gw_ip[16];
    char oif[32];
    nfc_t *notification_chain;
    struct _routing_table_entry *prev;
    struct _routing_table_entry *next;
} routing_table_entry_t;

typedef struct
{
    routing_table_entry_t *head;
} routing_table_t;

void
routing_table_init(routing_table_t *routing_table);

routing_table_entry_t *
routing_table_add_entry(routing_table_t *routing_table, char *dest_ip, char mask, char *gw_ip, char *oif);

bool
routing_table_delete_enty(routing_table_t *routing_table, char *dest_ip, char mask);

routing_table_entry_t *
routing_table_look_up_entry(routing_table_t *routing_table, char *dest, char mask);

void
routing_table_dump(routing_table_t *routing_table);

void
routing_table_register_for_notification(routing_table_t *routing_table,
                                        routing_table_entry_keys_t *key,
                                        size_t key_size,
                                        nfc_app_cb app_cb,
                                        int subscriptor_id);

#define ITERATE_ROUTING_TABLE_BEGIN(routing_table_ptr, routing_table_entry_pointer)     \
{                                                                                       \
    routing_table_entry_t *_next_routing_table_entry;                                   \
    for((routing_table_entry_pointer) = (routing_table_ptr)->head;                      \
        routing_table_entry_pointer;                                                    \
        routing_table_entry_pointer = _next_routing_table_entry){                       \
        _next_routing_table_entry = (routing_table_entry_pointer)->next;

#define ITERATE_ROUTING_TABLE_END   }}

#endif /* ROUTING_TABLE_H */
