#ifndef NOTIFICATION_CHAIN_H
#define NOTIFICATION_CHAIN_H

#include "glthreads.h"
#include <stddef.h>
#include <stdbool.h>

#define MAX_NOTIFY_KEY_SIZE     64
#define MAX_NFC_NAME_SIZE       64

typedef enum
{
    NFC_UNKNOWN,
    NFC_SUB,
    NFC_ADD,
    NFC_MOD,
    NFC_DEL,
}nfc_type_update_t;

typedef void (*nfc_app_cb)(int, void *, size_t, nfc_type_update_t);

typedef struct
{
    char key[MAX_NOTIFY_KEY_SIZE];
    size_t key_size;
    bool is_key_set;
    int subscriptor_id;
    nfc_app_cb app_cb;
    glthread_node_t glnode;
} nfc_element_t;

typedef struct
{
    char nfc_name[MAX_NFC_NAME_SIZE];
    glthread_t list;
} nfc_t;

void
nfc_register(nfc_t *nfc, nfc_element_t *nfc_element);

void
nfc_invoke(nfc_t *nfc,
           void *arg, size_t arg_size, nfc_type_update_t nfc_type_update,
           char *key, size_t key_size);

nfc_t *
nfc_create(char *nfc_name, int size_name);

void
nfc_delete(nfc_t *nfc);

char *
nfc_get_str_operation_code(nfc_type_update_t nfc_operation_code);

#endif /* NOTIFICATION_CHAIN_H */
