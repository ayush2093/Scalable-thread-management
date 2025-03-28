#include "MessageQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TIMEOUT_SEC 2

void* timeout_checker(void* arg) {
    MessageQueue* queue = (MessageQueue*)arg;
    while (1) {
        pthread_mutex_lock(&queue->mutex);
        if (!queue->running && queue->front == NULL) {
            pthread_mutex_unlock(&queue->mutex);
            return NULL;
        }
        Task *current = queue->front, *prev = NULL;
        while (current != NULL) {
            if (current->start_time > 0 && time(NULL) - current->start_time > TIMEOUT_SEC) {
                printf("Task with priority %d timed out\n", current->priority);
                if (prev == NULL) {
                    queue->front = current->next;
                } else {
                    prev->next = current->next;
                }
                if (current->next == NULL) queue->rear = prev;
                Task *temp = current;
                current = current->next;
                free(temp->arg);
                free(temp);
                continue;
            }
            prev = current;
            current = current->next;
        }
        pthread_mutex_unlock(&queue->mutex);
        sleep(2);
    }
    return NULL;
}

void init_message_queue(MessageQueue *queue) {
    queue->front = queue->rear = NULL;
    queue->running = 1;
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
    pthread_create(&queue->timeout_thread, NULL, timeout_checker, queue);
    pthread_detach(queue->timeout_thread);
}

void push_message(MessageQueue *queue, void (*function)(void*), void *arg, int priority, void (*callback)(void*)) {
    Task *task = (Task *)malloc(sizeof(Task));
    task->function = function;
    task->arg = arg;
    task->priority = priority;
    task->callback = callback;
    task->start_time = 0;
    task->timed_out = 0;
    task->next = NULL;

    pthread_mutex_lock(&queue->mutex);
    if (queue->rear == NULL) {
        queue->front = queue->rear = task;
    } else if (queue->front->priority < priority) {
        task->next = queue->front;
        queue->front = task;
    } else {
        Task *current = queue->front;
        while (current->next != NULL && current->next->priority >= priority) {
            current = current->next;
        }
        task->next = current->next;
        current->next = task;
        if (task->next == NULL) queue->rear = task;
    }
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

Task *pop_message(MessageQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    while (queue->front == NULL && queue->running) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }
    if (!queue->running && queue->front == NULL) {
        pthread_mutex_unlock(&queue->mutex);
        return NULL;
    }
    Task *task = queue->front;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    task->start_time = time(NULL);
    pthread_mutex_unlock(&queue->mutex);
    return task;
}

void shutdown_message_queue(MessageQueue *queue) {
    pthread_mutex_lock(&queue->mutex);
    queue->running = 0;
    pthread_cond_broadcast(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}
