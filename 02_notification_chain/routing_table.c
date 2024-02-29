#include "routing_table.h"
#include "notification_chain.h"
#include "utilities.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

static inline void
routing_table_entry_remove(routing_table_t *routing_table, routing_table_entry_t *routing_table_entry)
{
    if(NULL == routing_table_entry->prev)
    {
        if(NULL != routing_table_entry->next)
        {
            routing_table_entry->next->prev = NULL;
            routing_table->head = routing_table_entry->next;
            routing_table_entry->next = NULL;
            return;
        }
        routing_table->head = NULL;
        return;
    }

    if(NULL == routing_table_entry->next)
    {
        routing_table_entry->prev->next = NULL;
        routing_table_entry->prev = NULL;
        return;
    }

    routing_table_entry->prev->next = routing_table_entry->next;
    routing_table_entry->next->prev = routing_table_entry->prev;
    routing_table_entry->prev = NULL;
    routing_table_entry->next = NULL;
}

void
routing_table_init(routing_table_t *routing_table)
{
    routing_table->head = NULL;
}

routing_table_entry_t *
routing_table_add_entry(routing_table_t *routing_table, char *dest_ip, char mask, char *gw_ip, char *oif)
{
    routing_table_entry_t *routing_table_entry, *tmp_head;

    routing_table_entry = routing_table_look_up_entry(routing_table, dest_ip, mask);

    if(NULL == routing_table_entry)
    {
        routing_table_entry = calloc(1, sizeof(routing_table_entry_t));
        if(NULL == routing_table_entry)
        {
            handle_error("calloc");
        }

        if(NULL == memcpy(routing_table_entry->routing_table_entry_keys.dest, dest_ip,
                          sizeof(routing_table_entry->routing_table_entry_keys.dest)))
        {
            handle_error("memcpy");
        }
        routing_table_entry->routing_table_entry_keys.mask = mask;

        tmp_head = routing_table->head;
        routing_table->head = routing_table_entry;
        routing_table_entry->prev = 0;
        routing_table_entry->next = tmp_head;
        if(NULL != tmp_head)
            tmp_head->prev = routing_table_entry;

        routing_table_entry->notification_chain = nfc_create(NULL, 0);
    }

    /* Update the entry if it exists */
    if(NULL != gw_ip)
    {
        if(NULL == memcpy(routing_table_entry->gw_ip, gw_ip, sizeof(routing_table_entry->gw_ip)))
        {
            handle_error("memcpy");
        }
    }
    if(NULL != oif)
    {
        if(NULL == memcpy(routing_table_entry->oif, oif, sizeof(routing_table_entry->oif)))
        {
            handle_error("memcpy");
        }
    }

    if((NULL != gw_ip) || (NULL != oif))
    {
        nfc_invoke(routing_table_entry->notification_chain,
                   (char*)routing_table_entry, 
                   sizeof(routing_table_entry_t),
                   NFC_MOD, NULL, 0);
    }

    return routing_table_entry;
}

bool
routing_table_delete_enty(routing_table_t *routing_table, char *dest_ip, char mask)
{
    routing_table_entry_t *routing_table_entry = NULL;

    ITERATE_ROUTING_TABLE_BEGIN(routing_table, routing_table_entry){
        if(0 == strncmp(routing_table_entry->routing_table_entry_keys.dest, dest_ip,
                        sizeof(routing_table_entry->routing_table_entry_keys.dest)) &&
           routing_table_entry->routing_table_entry_keys.mask == mask)
        {
            routing_table_entry_remove(routing_table, routing_table_entry);
            nfc_invoke(routing_table_entry->notification_chain,
                   (char*)routing_table_entry, 
                   sizeof(routing_table_entry_t),
                   NFC_DEL, NULL, 0);
            nfc_delete(routing_table_entry->notification_chain);
            free(routing_table_entry->notification_chain);
            routing_table_entry->notification_chain = NULL;
            free(routing_table_entry);
            return true;
        }
    }ITERATE_ROUTING_TABLE_END;

    return false;
}

routing_table_entry_t *
routing_table_look_up_entry(routing_table_t *routing_table, char *dest, char mask)
{
    routing_table_entry_t *routing_table_entry = NULL;

    ITERATE_ROUTING_TABLE_BEGIN(routing_table, routing_table_entry){
        if((0 == strncmp(routing_table_entry->routing_table_entry_keys.dest, dest,
                         sizeof(routing_table_entry->routing_table_entry_keys.dest))) &&
            mask == routing_table_entry->routing_table_entry_keys.mask)
        {
            return routing_table_entry;
        }
    }ITERATE_ROUTING_TABLE_END;

    return NULL;
}

void
routing_table_dump(routing_table_t *routing_table)
{
    routing_table_entry_t *routing_table_entry = NULL;
    nfc_element_t *nfc_element;
    glthread_t *notification_chain;

    ITERATE_ROUTING_TABLE_BEGIN(routing_table, routing_table_entry){
        printf("%-20s %-4d %-20s %s\n", routing_table_entry->routing_table_entry_keys.dest,
                                        routing_table_entry->routing_table_entry_keys.mask,
                                        routing_table_entry->gw_ip,
                                        routing_table_entry->oif);
        printf("\tPrinting Subscribers: ");

        notification_chain = &routing_table_entry->notification_chain->list;
        GLTHREAD_ITERATE_BEGIN(notification_chain, nfc_element_t, nfc_element){
            printf("%d ", nfc_element->subscriptor_id);
        }GLTHREAD_ITERATE_END;
        printf("\n");
    }ITERATE_ROUTING_TABLE_END;
}

void
routing_table_register_for_notification(routing_table_t *routing_table,
                                        routing_table_entry_keys_t *key,
                                        size_t key_size,
                                        nfc_app_cb app_cb,
                                        int subscriptor_id)
{
    routing_table_entry_t *routing_table_entry;
    nfc_element_t nfc_element;
    memset(&nfc_element, 0, sizeof(nfc_element_t));

    assert(key_size <= MAX_NOTIFY_KEY_SIZE);

    routing_table_entry = routing_table_look_up_entry(routing_table, key->dest, key->mask);
    if(NULL == routing_table_entry)
    {
        routing_table_entry = routing_table_add_entry(routing_table, key->dest, key->mask, NULL, NULL);
    }
    else
    {
        app_cb(subscriptor_id, routing_table_entry, sizeof(routing_table_entry_t), NFC_ADD);
    }

    nfc_element.subscriptor_id = subscriptor_id;
    nfc_element.app_cb = app_cb;
    nfc_register(routing_table_entry->notification_chain, &nfc_element);
}
