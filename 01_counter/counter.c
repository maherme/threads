/*
 * This is a program that decrements a numeric value  which is given as parameter.
 * The program creates as many threads as the value given as parameter, and uses these threads for
 * decrementing the numeric value.
 *
 * So an execution like this: "$ a.out 5" will give this output:
 * Number of threads to be created: 5
 * Thread id is: 5
 * Thread id is: 4
 * Thread id is: 3
 * Thread id is: 2
 * Thread id is: 1
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BASE_INPUT_NUMBER   10

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define print_error_and_exit(msg) \
        do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while(0)

static long thread_id = 0;

void
check_args(int argc, char **argv);

long
get_input_argument(char *input);

void *
thread_start(void *arg);

void
thread_create(pthread_t *pthread_handle, long thread_id);

void
thread_join(pthread_t thread_handler);

int
main(int argc, char **argv)
{
    char *str;
    long number_threads;
    pthread_t thread_handler;

    check_args(argc, argv);

    str = argv[1];
    number_threads = get_input_argument(str);
    printf("Number of threads to be created: %ld\n", number_threads);

    thread_create(&thread_handler, number_threads);
    thread_join(thread_handler);

    exit(EXIT_SUCCESS);
}

void
check_args(int argc, char **argv)
{
    char buffer[100];

    if(argc != 2)
    {
        sprintf(buffer, "Usage: %s <counter_value>\n", argv[0]);
        print_error_and_exit(buffer);
    }
}

long
get_input_argument(char *input)
{
    char *endptr;
    long val;

    errno = 0;
    val = strtol(input, &endptr, BASE_INPUT_NUMBER);

    if(0 != errno)
    {
        handle_error("strol");
    }

    if((endptr == input) || (val < 0))
    {
        print_error_and_exit("Input argument must be a positive integer number\n");
    }

    return val;
}

void *
thread_start(void *arg)
{
    pthread_t thread_handler;
    long local_thread_id = ++thread_id;

    long number_threads = *(long*)arg;
    free(arg);

    if(local_thread_id < number_threads)
    {
        thread_create(&thread_handler, number_threads);
        thread_join(thread_handler);
    }

    printf("Thread id is: %ld\n", local_thread_id);

    return NULL;
}

void
thread_create(pthread_t *pthread_handle, long thread_id)
{
    int s;
    pthread_attr_t attr;
    long *_thread_id;

    _thread_id = calloc(1, sizeof(long));
    if(_thread_id == NULL)
    {
        handle_error("calloc");
    }
    *_thread_id = thread_id;

    s = pthread_attr_init(&attr);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_init");
    }

    s = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if(0 != s)
    {
        handle_error_en(s, "pthread_attr_setdetachstate");
    }

    s = pthread_create(pthread_handle, &attr, thread_start, (void*)_thread_id);
    if(0 != s)
    {
        handle_error_en(s, "pthread_create");
    }
}

void
thread_join(pthread_t thread_handler)
{
    int s = pthread_join(thread_handler, NULL);

    if(0 != s)
    {
        handle_error_en(s, "pthread_join");
    }
}
