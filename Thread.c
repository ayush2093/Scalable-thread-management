#include <stdio.h>
#include <pthread.h>
#include "Thread.h"

void* thread_function(void* arg) {
    printf("Hello from the Anand,Ayush,Ravi!\n");
    return NULL;
}

void run_basic_thread_test() {
    pthread_t thread;
    int result = pthread_create(&thread, NULL, thread_function, NULL);
    if (result != 0) {
        printf("Error creating thread\n");
        return;
    }
    pthread_join(thread, NULL);
    printf("Thread finished, back in main!\n");
}
