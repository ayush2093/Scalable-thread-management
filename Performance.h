#ifndef PERFORMANCE_H
#define PERFORMANCE_H

typedef struct {
    double start_time;
    double end_time;
    double total_time;
    int tasks_completed;
} PerformanceMetrics;

double get_current_time();
void init_performance_metrics(PerformanceMetrics *metrics);
void start_timer(PerformanceMetrics *metrics);
void stop_timer(PerformanceMetrics *metrics);
void record_task_completion(PerformanceMetrics *metrics);
void print_performance_metrics(PerformanceMetrics *metrics);

#endif
