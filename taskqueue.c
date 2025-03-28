#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define THREAD_POOL_SIZE 4
#define TASK_COUNT 10

pthread_mutex_t lock;
pthread_cond_t cond;

int *taskQueue;
int taskIndex = 0;

void* workerThread(void* arg) {
    while (1) {
        pthread_mutex_lock(&lock);

        while (taskIndex >= TASK_COUNT) {
            pthread_cond_wait(&cond, &lock);
        }

        int task = taskQueue[taskIndex++];
        pthread_mutex_unlock(&lock);

        printf("Thread %ld processing task %d\n", pthread_self(), task);
        sleep(2);
    }
}


int main() {
    pthread_t threads[THREAD_POOL_SIZE];
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    taskQueue = (int*)malloc(TASK_COUNT * sizeof(int));

    for (int i = 0; i < TASK_COUNT; i++) {
        taskQueue[i] = i + 1;
    }

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&threads[i], NULL, workerThread, NULL);
    }

    pthread_mutex_lock(&lock);
    taskIndex = 0;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);

    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_join(threads[i], NULL);
    }

    free(taskQueue);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    return 0;
}
