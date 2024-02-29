#ifndef GLTHREADS_H
#define GLTHREADS_H

#include <stdbool.h>

#define offsetof(struct_name, field_name) \
        ((size_t)&((struct_name*)0)->field_name)

typedef struct glthread_node
{
    struct glthread_node *previous;
    struct glthread_node *next;
} glthread_node_t;

typedef struct
{
    glthread_node_t *head;
    unsigned int offset;
} glthread_t;

void
glthread_add(glthread_t *list, glthread_node_t *glnode);

void
glthread_remove(glthread_t *list, glthread_node_t *glnode);

void
glthread_init(glthread_t *list, unsigned int offset);

void
glthread_node_init(glthread_node_t *glnode);

bool
is_glthread_node_unlink(const glthread_node_t *glnode);

#define GLTHREAD_ITERATE_BEGIN(list_pointer, struct_type, ptr)          \
{                                                                       \
    glthread_node_t *current, *next = NULL;                             \
    for(current = list_pointer->head; current; current = next)          \
    {                                                                   \
        next = current->next;                                           \
        ptr = (struct_type*)((char*)current - list_pointer->offset);    \

#define GLTHREAD_ITERATE_END    }}

#endif /* GLTHREADS_H */
