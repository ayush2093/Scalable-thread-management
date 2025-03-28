#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <pthread.h>
#include <time.h>

typedef struct Task {
    void (*function)(void*);
    void* arg;
    int priority;
    void (*callback)(void*);
    time_t start_time;
    int timed_out;
    struct Task *next;
} Task;

typedef struct {
    Task *front;
    Task *rear;
    int running;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_t timeout_thread;
} MessageQueue;

void init_message_queue(MessageQueue *queue);
void push_message(MessageQueue *queue, void (*function)(void*), void *arg, int priority, void (*callback)(void*));
Task *pop_message(MessageQueue *queue);
void shutdown_message_queue(MessageQueue *queue);
void *timeout_checker(void *arg);

#endif
