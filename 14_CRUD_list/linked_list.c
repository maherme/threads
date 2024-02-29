#include "linked_list.h"
#include <stdlib.h>
#include <stdio.h>

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

linked_list_t *
init_singly_ll(void)
{
    linked_list_t * ll =  calloc(1, sizeof(linked_list_t));
    if(!ll) handle_error("calloc");
    return ll;
}

node_t *
singly_ll_init_node(void *data)
{
    node_t* node = calloc(1, sizeof(node_t));
    if(!node) handle_error("calloc");
    node->data = data;
    return node;
}

int
singly_ll_add_node(linked_list_t *ll, node_t *node)
{
    if(!ll || !node) return -1;
    if(!ll->head)
    {
        ll->head  = node;
        ll->node_count++;
        return 0;
    }

    node->next = ll->head;
    ll->head = node;
    ll->node_count++;
    return 0;
}

int
singly_ll_remove_node(linked_list_t *ll, node_t *node)
{
    if(!ll || !ll->head) return 0;
    if(!node)
    {
        fprintf(stderr, "%s(%d): Error: node is NULL\n", __FUNCTION__, __LINE__);
        return -1;
    }

    node_t *head = ll->head, *prev = NULL;
    if(head == node)
    {
        ll->head = head->next;
        ll->node_count--;
        node->next = NULL;
        return 0;
    }
    prev = head;
    head = head->next;
    for(unsigned int i = 0; i < ll->node_count; i++)
    {
        if(head != node)
        {
            prev = head;
            head = head->next;
            continue;
        }
        prev->next = head->next;
        head->next = NULL;
        ll->node_count--;
        return 0;
    }
    fprintf(stderr, "%s(%d): Error: node not found\n", __FUNCTION__, __LINE__);
    return -1;
}

void
singly_ll_delete_node_by_data_ptr(linked_list_t *ll, void *data)
{
    if(!data) return;
    node_t *node = singly_ll_get_node_by_data_ptr(ll, data);
    if(!node) return;
    singly_ll_remove_node(ll, node);
    free(node);
    node = NULL;
}

int
singly_ll_add_node_by_val(linked_list_t *ll, void *data)
{
    if(singly_ll_get_node_by_data_ptr(ll, data)) return -1;
    node_t *node = singly_ll_init_node(data);
    return singly_ll_add_node(ll, node);
}

node_t *
singly_ll_get_node_by_data_ptr(linked_list_t *ll, void *data)
{
    if(!ll || !ll->head) return NULL;
    node_t *head = ll->head;

    for(unsigned int i = 0; i < ll->node_count; i++)
    {
        if(data == head->data)
            return head;
        head = head->next;
    }
    return NULL;
}
