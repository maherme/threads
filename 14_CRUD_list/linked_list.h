#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct _node_t
{
    void *data;
    struct _node_t *next;
} node_t;

typedef struct
{
    unsigned int node_count;
    node_t *head;
    int (*comparision_fn)(void*, void*);
    int (*order_comparison_fn)(void*, void*);
} linked_list_t;

linked_list_t *
init_singly_ll(void);

node_t *
singly_ll_init_node(void *data);

int
singly_ll_add_node(linked_list_t *ll, node_t *node);

int
singly_ll_remove_node(linked_list_t *ll, node_t *node);

void
singly_ll_delete_node_by_data_ptr(linked_list_t *ll, void *data);

int
singly_ll_add_node_by_val(linked_list_t *ll, void *data);

node_t *
singly_ll_get_node_by_data_ptr(linked_list_t *ll, void *data);

#define ITERATE_LIST_BEGIN2(list_ptr, node_ptr, prev)   \
    {                                                   \
        node_t* _node_ptr = NULL; prev = NULL;          \
        node_ptr = list_ptr->head;                      \
        for(; NULL != node_ptr; node_ptr = _node_ptr) { \
            _node_ptr = node_ptr->next;

#define ITERATE_LIST_END2(list_ptr, node_ptr, prev)     \
        if(node_ptr) prev = node_ptr; }}

#endif /* LINKED_LIST_H */
