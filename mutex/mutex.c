#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define NR_LOOP 10000

static int count  = 10;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void *thread1_routine(void *arg){

    for(int i = 0; i < NR_LOOP; i++){
        pthread_mutex_lock(&mutex); /* Using lock the count++; is an atomic operation */
        count++;
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}

static void *thread2_routine(void *arg){

    for(int i = 0; i < NR_LOOP; i++){
        pthread_mutex_lock(&mutex); /* Using lock the count++; is an atomic operation */
        count--;
        pthread_mutex_unlock(&mutex);
    }

    return 0;
}

static void error_exit(char *strerr){
    perror(strerr);
    exit(1);
}

int main(int argc, char *argv[]){

    pthread_t thread1, thread2;

    if(pthread_create(&thread1, NULL, thread1_routine, NULL) != 0){
        error_exit("thread1 cannot be created");
    }

    if(pthread_create(&thread2, NULL, thread2_routine, NULL) != 0){
        error_exit("thread2 cannot be created");
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Count value: %d\n", count);

    return 0;
}
