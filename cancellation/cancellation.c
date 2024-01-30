#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define NUMBER_THREADS          5
#define FILE_NAME_MAX           64
#define STRINT_TO_WRITE_MAX     64

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
#define handle_error(msg) \
        do { perror(msg); exit(EXIT_FAILURE); } while (0)
#define print_error_and_exit(msg) \
        do { fprintf(stderr, msg); exit(EXIT_FAILURE); } while(0)

pthread_t threads[NUMBER_THREADS];

static void
memory_cleanup_handler(void *arg)
{
    printf("%s invoked ...\n", __FUNCTION__);
    free(arg);
}

static void
file_cleanup_handler(void *arg)
{
    printf("%s invoked ...\n", __FUNCTION__);
    fclose((FILE *)arg);
}

static void *
write_info_file(void *arg)
{
    char file_name[FILE_NAME_MAX];
    char string_to_write[STRINT_TO_WRITE_MAX];
    int len;
    int count = 0;
    FILE *file;
    int thread_id = *(int*)arg;
    int s;
    size_t written_elements;

    s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if(0 != s)
    {
        handle_error_en(s, "pthread_setcancelstate");
    }

    s = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    if(0 != s)
    {
        handle_error_en(s, "pthread_setcanceltype");
    }

    pthread_cleanup_push(memory_cleanup_handler, arg);

    sprintf(file_name, "thread_%d.txt", thread_id);

    file = fopen(file_name, "w");
    if(!file)
    {
        fprintf(stderr, "Error: could not open log file %s, errno = %d\n", file_name, errno);
        pthread_exit(NULL);
    }

    pthread_cleanup_push(file_cleanup_handler, file);

    while(1)
    {
        len = sprintf(string_to_write, "%d : I am thread %d\n", count++, thread_id);
        written_elements = fwrite(string_to_write, sizeof(char), len, file);
        if(written_elements != len)
        {
            perror("fwrite");
            pthread_exit(NULL);
        }
        fflush(file);
        sleep(1);
        pthread_testcancel();
    }

    pthread_cleanup_pop(1);
    pthread_cleanup_pop(1);

    return NULL;
}

int
main(int argc, char **argv)
{
    int *thread_id;
    int choice;
    int thread_id_input;
    int s;

    for(int i = 0; i < NUMBER_THREADS; i++)
    {
        thread_id = calloc(1, sizeof(*thread_id));
        if(!thread_id)
        {
            handle_error("calloc");
        }
        *thread_id = i;
        s = pthread_create(&threads[i], NULL, write_info_file, thread_id);
        if(0 != s)
        {
            handle_error_en(s, "pthread_create");
        }
    }

    while(1)
    {
        printf("1. Cancel the thread\n");
        scanf("%d", &choice);
        printf("Enter thread id [0-%d]: ", NUMBER_THREADS - 1);
        scanf("%d", &thread_id_input);
        if(thread_id_input < 0 || thread_id_input >= NUMBER_THREADS)
        {
            handle_error("Invalid thread id");
        }
        printf("\n");

        switch(choice)
        {
            case 1:
                s = pthread_cancel(threads[thread_id_input]);
                if(0 != s)
                {
                    handle_error_en(s, "pthread_cancel");
                }
                break;
            default:
                continue;
        }
    }

    exit(EXIT_SUCCESS);
}
