
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "Thread.h"
#include "MessageQueue.h"
#include "Performance.h"

#define NUM_CONSUMERS 3
#define PREDEFINED_TASKS 10

typedef struct {
    pthread_t* threads;
    int* busy;
    int thread_count;
    MessageQueue queue;
    PerformanceMetrics metrics;
} ThreadPool;

void* worker(void* arg) {
    ThreadPool* pool = (ThreadPool*)arg;
    int id = -1;
    pthread_mutex_lock(&pool->queue.mutex);
    for (int i = 0; i < pool->thread_count; i++) {
        if (pthread_self() == pool->threads[i]) id = i;
    }
    pthread_mutex_unlock(&pool->queue.mutex);

    while (1) {
        Task *task = pop_message(&pool->queue);
        if (task == NULL) break;
        pool->busy[id] = 1;
        if (!task->timed_out) {
            task->function(task->arg);
            if (task->callback) task->callback(task->arg);
        }
        record_task_completion(&pool->metrics);
        pool->busy[id] = 0;
        free(task);
    }
    return NULL;
}

ThreadPool* create_thread_pool(int num_threads) {
    ThreadPool* pool = malloc(sizeof(ThreadPool));
    pool->thread_count = num_threads;
    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    pool->busy = malloc(sizeof(int) * num_threads);
    init_message_queue(&pool->queue);
    init_performance_metrics(&pool->metrics);
    for (int i = 0; i < num_threads; i++) {
        pool->busy[i] = 0;
        pthread_create(&pool->threads[i], NULL, worker, pool);
        pthread_detach(pool->threads[i]);
    }
    return pool;
}

void destroy_thread_pool(ThreadPool* pool) {
    shutdown_message_queue(&pool->queue);
    free(pool->threads);
    free(pool->busy);
    free(pool);
}

void sample_task(void* arg) {
    int num = *(int*)arg;
    printf("Task %d running with priority %d\n", num, num);
    sleep(5);
}

void sample_callback(void* arg) {
    int num = *(int*)arg;
    printf("Task %d completed\n", num);
}

int main() {
    printf("Running basic thread test...\n");
    run_basic_thread_test();

    ThreadPool* pool = create_thread_pool(NUM_CONSUMERS);
    start_timer(&pool->metrics);

    printf("Adding predefined tasks...\n");
    for (int i = 0; i < PREDEFINED_TASKS; i++) {
        int *num = (int *)malloc(sizeof(int));
        *num = i + 1;
        push_message(&pool->queue, sample_task, num, i % 3, sample_callback);
        printf("Added Task %d with priority %d\n", i + 1, i % 3);
        usleep(500000);
    }

    sleep(15);
    printf("Busy threads: ");
    for (int i = 0; i < pool->thread_count; i++) {
        if (pool->busy[i]) printf("%d ", i);
    }
    printf("\n");

    stop_timer(&pool->metrics);
    print_performance_metrics(&pool->metrics);

    destroy_thread_pool(pool);
    printf("Thread pool shut down\n");

    // Asking for exit confirmation
    char user_input;
    do {
        printf("All tasks completed. Do you want to exit? (Y/N): ");
        scanf(" %c", &user_input);
        
        if (user_input == 'Y' || user_input == 'y') {
            printf("Exiting program...\n");
            break;
        } else if (user_input == 'N' || user_input == 'n') {
            printf("Restarting tasks...\n");
            // Optionally, restart the task execution loop here if needed
        } else {
            printf("Invalid input. Please enter Y or N.\n");
        }
    } while (1);
    
    return 0;
}
