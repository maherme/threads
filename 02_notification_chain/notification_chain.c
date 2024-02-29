#include "notification_chain.h"
#include "glthreads.h"
#include "utilities.h"
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

void
nfc_register(nfc_t *nfc, nfc_element_t *nfc_element)
{
    nfc_element_t *new_nfc_element = calloc(1, sizeof(nfc_element_t));
    if(NULL == new_nfc_element)
    {
        handle_error("calloc");
    }

    if(NULL == memcpy(new_nfc_element, nfc_element, sizeof(nfc_element_t)))
    {
        handle_error("memcpy");
    }

    glthread_node_init(&new_nfc_element->glnode);
    glthread_add(&nfc->list, &new_nfc_element->glnode);
}

void
nfc_invoke(nfc_t *nfc,
           void *arg, size_t arg_size, nfc_type_update_t nfc_type_update,
           char *key, size_t key_size)
{
    nfc_element_t *tmp_node;
    glthread_t *tmp_list = &nfc->list;

    assert(key_size <= MAX_NOTIFY_KEY_SIZE);

    GLTHREAD_ITERATE_BEGIN(tmp_list, nfc_element_t, tmp_node){
        if(!(key && key_size && tmp_node->is_key_set && (key_size == tmp_node->key_size)))
        {
            tmp_node->app_cb(tmp_node->subscriptor_id, arg, arg_size, nfc_type_update);
        }
        else
        {
            if(0 == memcmp(key, tmp_node->key, key_size))
            {
                tmp_node->app_cb(tmp_node->subscriptor_id, arg, arg_size, nfc_type_update);
            }
        }
    }GLTHREAD_ITERATE_END;
}

nfc_t *
nfc_create(char *nfc_name, int size_name)
{
    nfc_t* new_nfc = calloc(1, sizeof(nfc_t));
    if(NULL == new_nfc)
    {
        handle_error("calloc");
    }

    if((NULL != nfc_name) && (MAX_NFC_NAME_SIZE > size_name))
    {
        if (NULL == memcpy(new_nfc->nfc_name, nfc_name, sizeof(size_name)))
        {
            handle_error("memcpy");
        }
    }

    glthread_init(&new_nfc->list, offsetof(nfc_element_t, glnode));

    return new_nfc;
}

void
nfc_delete(nfc_t *nfc)
{

    nfc_element_t *tmp_node;
    glthread_t *tmp_list = &nfc->list;

    GLTHREAD_ITERATE_BEGIN(tmp_list, nfc_element_t, tmp_node){
        free(tmp_node);
    }GLTHREAD_ITERATE_END;

    free(nfc);
}

char *
nfc_get_str_operation_code(nfc_type_update_t nfc_operation_code)
{
    switch(nfc_operation_code)
    {
        case NFC_UNKNOWN:
            return "NFC_UNKNOWN";
        case NFC_SUB:
            return "NFC_SUB";
        case NFC_ADD:
            return "NFC_ADD";
        case NFC_MOD:
            return "NFC_MOD";
        case NFC_DEL:
            return "NFC_DEL";
        default:
            return NULL;
    }
}
