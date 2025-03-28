#include "Performance.h"
#include <stdio.h>
#include <time.h> // Added for clock_gettime, struct timespec, and CLOCK_MONOTONIC

double get_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void init_performance_metrics(PerformanceMetrics *metrics) {
    metrics->start_time = 0;
    metrics->end_time = 0;
    metrics->total_time = 0;
    metrics->tasks_completed = 0;
}

void start_timer(PerformanceMetrics *metrics) {
    metrics->start_time = get_current_time();
}

void stop_timer(PerformanceMetrics *metrics) {
    metrics->end_time = get_current_time();
    metrics->total_time = metrics->end_time - metrics->start_time;
}

void record_task_completion(PerformanceMetrics *metrics) {
    metrics->tasks_completed++;
}

void print_performance_metrics(PerformanceMetrics *metrics) {
    printf("Performance Metrics:\n");
    printf("Total Time: %.2f seconds\n", metrics->total_time);
    printf("Tasks Completed: %d\n", metrics->tasks_completed);
    if (metrics->total_time > 0) {
        printf("Throughput: %.2f tasks/second\n", metrics->tasks_completed / metrics->total_time);
    }
}
