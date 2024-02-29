#include "glthreads.h"
#include <stddef.h>

static void
glthread_add_next(glthread_node_t *current_node, glthread_node_t *new_node)
{
    if(new_node->next == NULL)
    {
        new_node->next = current_node;
        current_node->previous = new_node;
    }
}

void
glthread_add(glthread_t *list, glthread_node_t *glnode)
{
    glnode->previous = NULL;
    glnode->next = NULL;

    if(list->head == NULL)
    {
        list->head = glnode;
        return;
    }

    glthread_node_t *head = list->head;
    glthread_add_next(head, glnode);
    list->head = glnode;
}

static void
glthread_remove_node(glthread_node_t *glnode)
{
    /* Check if the node is the first one in the list */
    if(!glnode->previous)
    {
        /* Check if the node is not the onlyone in the list */
        if(glnode->next)
        {
            glnode->next->previous = NULL;
            glnode->next = NULL;
        }
        return;
    }

    /* Check if the node is the last node in the list */
    if(!glnode->next)
    {
        glnode->previous->next = NULL;
        glnode->previous = NULL;
        return;
    }

    glnode->previous->next = glnode->next;
    glnode->next->previous = glnode->previous;
    glnode->next = NULL;
    glnode->previous = NULL;
}

void
glthread_remove(glthread_t *list, glthread_node_t *glnode)
{
    /* Check if the node is the first node in the list */
    if(list->head == glnode)
    {
        list->head = list->head->next;
    }

    glthread_remove_node(glnode);
}

void
glthread_init(glthread_t *list, unsigned int offset)
{
    list->head = NULL;
    list->offset = offset;
}

void
glthread_node_init(glthread_node_t *glnode)
{
    glnode->next = NULL;
    glnode->previous = NULL;
}

bool
is_glthread_node_unlink(const glthread_node_t *glnode)
{
    return (!glnode->previous && !glnode->next);
}
