#include "student_list.h"
#include "ref_count.h"
#include "linked_list.h"
#include "threads.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)

static int student_object_count = 0;

student_t *
student_malloc(int roll_no)
{
    student_t *new = (student_t *)calloc(1, sizeof(student_t));
    if(!new) handle_error("calloc");
    new->roll_no = roll_no;
    new->total_marks = 0;
    ref_count_init(&new->ref_count);
    thread_rwlock_init(&new->rw_lock);
    student_object_count++;
    return new;
}

void
student_destroy(student_t *student)
{
    assert(0 == student->ref_count.ref_count);
    ref_count_destroy(&student->ref_count);
    thread_rwlock_destroy(&student->rw_lock);
    free(student);
    student_object_count++;
}

student_t *
student_list_lookup(student_list_t *student_list, int roll_no)
{
    student_t *student;
    node_t *node1, *node2;
    (void)node2;

    ITERATE_LIST_BEGIN2(student_list->list, node1, node2) {
        student = (student_t *)(node1->data);
        if(roll_no == student->roll_no) return student;
    } ITERATE_LIST_END2(student_list->list, node1, node2);

    return NULL;
}

bool
student_list_insert(student_list_t *student_list, student_t *student)
{
    student_t *new = student_list_lookup(student_list, student->roll_no);
    if(new) return false;
    singly_ll_add_node_by_val(student_list->list, student);
    return true;
}

student_t *
student_list_remove(student_list_t *student_list, int roll_no)
{
    student_t *student = student_list_lookup(student_list, roll_no);
    if(!student) return NULL;
    singly_ll_delete_node_by_data_ptr(student_list->list, student);
    return student;
}
