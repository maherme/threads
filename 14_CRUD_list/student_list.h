#ifndef STUDENT_LIST_H
#define STUDENT_LIST_H

#include "ref_count.h"
#include "linked_list.h"
#include <pthread.h>

typedef struct
{
    int roll_no;
    int total_marks;
    ref_count_t ref_count;
    pthread_rwlock_t rw_lock;
}student_t;

typedef struct
{
    linked_list_t *list;
    pthread_rwlock_t rw_lock;
}student_list_t;

student_t *
student_malloc(int roll_no);

void
student_destroy(student_t *student);

student_t *
student_list_lookup(student_list_t *student_list, int roll_no);

bool
student_list_insert(student_list_t *student_list, student_t *student);

student_t *
student_list_remove(student_list_t *student_list, int roll_no);

#endif /* STUDENT_LIST_H */
