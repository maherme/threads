#include "threads.h"
#include "ref_count.h"
#include "student_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_ROLL_NO 10
#define INCR        5

static student_list_t student_list;

static void *
reader_fn(void *arg)
{
    (void)arg;
    student_t *student;
    int roll_no;

    while(1)
    {
        roll_no = rand()%MAX_ROLL_NO;
        thread_rwlock_rdlock(&student_list.rw_lock);
        student = student_list_lookup(&student_list, roll_no);

        if(!student)
        {
            printf("READ TH: Roll No %d do not exist\n", roll_no);
            thread_rwlock_unlock(&student_list.rw_lock);
            continue;
        }

        thread_using_object(&student->ref_count);
        thread_rwlock_rdlock(&student->rw_lock);
        thread_rwlock_unlock(&student_list.rw_lock);
        printf("READ TH: Roll No %d is read, total marks = %d\n", roll_no, student->total_marks);
        thread_rwlock_unlock(&student->rw_lock);
        if(thread_using_object_done(&student->ref_count))
        {
            printf("READ TH: Roll no %d read done\n", roll_no);
            student_destroy(student);
            printf("READ TH: Roll no %d destroyed\n", roll_no);
        }
        else
        {
            /* Never attempt to access the student object after this line in the program */
        }
    }

    return NULL;
}

static void *
update_fn(void *arg)
{
    (void)arg;
    student_t *student;
    int roll_no;

    while(1)
    {
        roll_no = rand()%MAX_ROLL_NO;
        thread_rwlock_rdlock(&student_list.rw_lock);
        student = student_list_lookup(&student_list, roll_no);

        if(!student)
        {
            printf("UPDATE TH: Roll No %d do not exist\n", roll_no);
            thread_rwlock_unlock(&student_list.rw_lock);
            continue;
        }

        thread_using_object(&student->ref_count);
        thread_rwlock_wrlock(&student->rw_lock);
        thread_rwlock_unlock(&student_list.rw_lock);
        printf("UPDATE TH: Roll No %d is read, total marks = %d\n",
               roll_no, student->total_marks);

        int old_marks = student->total_marks;
        student->total_marks += INCR;
        student->total_marks = student->total_marks%100;
        printf("UPDATE TH: Roll no %d, increment %d, marks update %d -> %d\n",
               student->roll_no, INCR, old_marks, student->total_marks);

        thread_rwlock_unlock(&student->rw_lock);
        if(thread_using_object_done(&student->ref_count))
        {
            printf("UPDATE TH: Roll no %d read done\n", roll_no);
            student_destroy(student);
            printf("UPDATE TH: Roll no %d destroyed\n", roll_no);
        }
        else
        {
            /* Never attempt to access the student object after this line in the program */
        }
    }

    return NULL;
}

static void *
create_fn(void *arg)
{
    (void)arg;
    int roll_no;
    student_t *student;

    while(1)
    {
        roll_no = rand()%MAX_ROLL_NO;
        thread_rwlock_wrlock(&student_list.rw_lock);
        student = student_list_lookup(&student_list, roll_no);

        if(student)
        {
            printf("CREATE TH: Roll No %d creation failed, already exist\n", roll_no);
            continue;
        }

        student = student_malloc(roll_no);
        student_list_insert(&student_list, student);
        ref_count_inc(&student->ref_count);
        printf("CREATE TH: Roll No %d creation success\n", roll_no);
        thread_rwlock_unlock(&student_list.rw_lock);
    }

    return NULL;
}

static void*
delete_fn(void *arg)
{
    (void)arg;
    int roll_no;
    student_t *student;

    while(1)
    {
        roll_no = rand()%MAX_ROLL_NO;
        thread_rwlock_wrlock(&student_list.rw_lock);
        student = student_list_remove(&student_list, roll_no);

        if(!student)
        {
            printf("DELETE TH: Roll No %d deletion failed, do not exist\n", roll_no);
            thread_rwlock_unlock(&student_list.rw_lock);
            continue;
        }

        thread_using_object(&student->ref_count);
        assert(!ref_count_dec(&student->ref_count));
        thread_rwlock_unlock(&student_list.rw_lock);
        printf("DELETE TH: Roll No %d removal success\n", roll_no);

        if(thread_using_object_done(&student->ref_count))
        {
            student_destroy(student);
            printf("DELETE TH: Roll No %d destroyed\n", roll_no);
        }
        else
        {
            printf("DELETE TH: Roll No %d deletion deferred\n", roll_no);
        }
    }

    return NULL;
}

int
main()
{
    exit(EXIT_SUCCESS);
}
