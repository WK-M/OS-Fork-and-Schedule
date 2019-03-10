#ifndef SHARED_H
#define SHARED_H

typedef struct process_control_block {
    char process_name[malloc(sizeof(char)*10];
    int pid;
    int state;
    int priority;
    int is_interruptable;
    int est_total_run_time;
    int est_remaining_run_time;
} PCB;

#endif
