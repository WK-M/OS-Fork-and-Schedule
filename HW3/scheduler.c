/*
 * Kendall Molas
 * CSc 33200
 * Project 3
 * Professor Gordon
 */
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 

#define NUMBER_OF_PCBS 14 
#define CHARACTER_LENGTH 80
#define BUFFER_SIZE 120

typedef struct queue Queue;
typedef struct process_control_block PCB;
typedef struct Statistics info;

// Self-defined boolean
typedef enum {false, true} bool;

/* Data Structures
 * PCB - holds info for the processes
 * Queue - holds a fixed amount of PCBs
 */
typedef struct process_control_block {
    int arrival;
    char process_name[10];
    int pid;
    char state[5];
    int priority;
    int is_interruptable;
    int est_total_run_time;
    int est_remaining_run_time;

    // Used for passing into named pipe
    char BLOCK[8][100];
} PCB;

typedef struct queue {
    PCB *queue_size[NUMBER_OF_PCBS];
    unsigned int front;
    int rear;
    unsigned int current_size;
} Queue;

typedef struct stats {
    int pid[NUMBER_OF_PCBS];
    int turnaround_time;
    int wait_time;
} Statistics;


// Reads from text file
// Takes in filename and queue
void interpret_data_from_text_file(char *file, Queue *q) {
    FILE *fp;

    fp = fopen(file, "r");
    if (fp == NULL) {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Working\n");
    }

    int clock = 0;
    bool process_information_retreived = false;
    char file_buffer[BUFFER_SIZE]; 
    char *process_pointer;

    while (fgets(file_buffer, sizeof(file_buffer), fp) != NULL) {
        // Make a new process
        PCB *p = malloc(sizeof(PCB)); 

        // Get data from the text file and store information from text file into the process
        if (process_pointer != NULL) {
            process_pointer = strtok(file_buffer, ",");
            p->arrival = strtol(process_pointer, NULL, 10);

            if (process_pointer != NULL) {
                process_pointer = strtok(NULL, ",");
                strcpy(p->process_name, process_pointer);
            }

            if (process_pointer != NULL) {
                process_pointer = strtok(NULL, ",");
                p->pid = strtol(process_pointer, NULL , 10);
            }

            if (process_pointer != NULL) {
                process_pointer = strtok(NULL, ",");
                strcpy(p->state, process_pointer);
            }

            if (process_pointer != NULL) {
                process_pointer = strtok(NULL, ",");
                p->priority = strtol(process_pointer, NULL , 10);
            }

            if (process_pointer != NULL) {
                process_pointer = strtok(NULL, ",");
                p->is_interruptable = strtol(process_pointer, NULL , 10);
            }

            if (process_pointer != NULL) {
                process_pointer = strtok(NULL, ",");
                p->est_total_run_time = strtol(process_pointer, NULL , 10);
            }

            if (process_pointer != NULL) {
                process_pointer = strtok(NULL, ",");
                p->est_remaining_run_time = strtol(process_pointer, NULL , 10);
                process_information_retreived = true;
            }
        }

        // Store into the BLOCK that will be used for communication in the named pipe
        if (process_information_retreived == true) {
            sprintf(p->BLOCK[0], "%d", p->arrival);
            strcpy(p->BLOCK[1], p->process_name);
            sprintf(p->BLOCK[2], "%d", p->pid);
            strcpy(p->BLOCK[3], p->state);
            sprintf(p->BLOCK[4], "%d", p->priority);
            sprintf(p->BLOCK[5], "%d", p->is_interruptable);
            sprintf(p->BLOCK[6], "%d", p->est_total_run_time);
            sprintf(p->BLOCK[7], "%d", p->est_remaining_run_time);

            // Enqueue process
            q->queue_size[q->current_size] = p;
            process_information_retreived = false;
            q->current_size++;
        }
    }
    fclose(fp);
}

// Debugging purposes
// Display content of current queue
void current_queue(Queue *q) {
    for (int i = 0; i < q->current_size; i++) {
        if (q->queue_size[i] != NULL)
            printf("PCB %d: %d [%s] %d %s %d %d %d %d\n", i, q->queue_size[i]->arrival ,q->queue_size[i]->process_name, q->queue_size[i]->pid, 
                    q->queue_size[i]->state, q->queue_size[i]->priority, q->queue_size[i]->is_interruptable, 
                    q->queue_size[i]->est_total_run_time,q->queue_size[i]->est_remaining_run_time);
    }
}

// Add PCB into ready queue
void enqueue(Queue *q, PCB *p) {
    if (p->est_remaining_run_time > 0) {
        //printf("Process [%s] has returned to queue\n", p->process_name);
        q->queue_size[q->rear] = p;
    }
}

// Organize based on SJN algorithm
// Sort queue based on remaining run time
void runSJN(Queue *q) {
    // First specify the type of scheduler algorithm from argv
    PCB *min_process;
    for (int i = 0; i < q->current_size; i++) {
        min_process = q->queue_size[i];
        for (int j = i; j < q->current_size; j++) {
            if (min_process->est_total_run_time > q->queue_size[j]->est_total_run_time) {
                min_process = q->queue_size[j];
                PCB *temp_process = q->queue_size[i];
                q->queue_size[i] = min_process;
                q->queue_size[j] = temp_process;
            }
        }
    }
}

// Organize based on Priority Algorithm
// Sort queue based on remaining run time
void runPriority(Queue *q) {
    // First specify the type of scheduler algorithm from argv
    PCB *process_with_low_priority;
    for (int i = 0; i < q->current_size; i++) {
        process_with_low_priority = q->queue_size[i];
        for (int j = i; j < q->current_size; j++) {
            if (process_with_low_priority->priority > q->queue_size[j]->priority) {
                process_with_low_priority = q->queue_size[j];
                PCB *temp_process = q->queue_size[i];
                q->queue_size[i] = process_with_low_priority;
                q->queue_size[j] = temp_process;
            }
        }
    }
}

// Returns PCB in the front of the queue
PCB *dequeue(Queue *q) {
    q->front = 0;
    q->rear = q->current_size - 1;
    PCB *dequeued_process = q->queue_size[q->front];

    // Move n-1 processes forward
    if (q->queue_size[q->current_size-1] != NULL) {
        for (int i = 0; i < q->current_size - 1; i++) {
            q->queue_size[i] = q->queue_size[i+1];
        }
        q->queue_size[q->rear] = NULL;

    }
    //printf("\nProcess [%s]: dequeued\n", dequeued_process->process_name);

    return dequeued_process;
}

int main(int argc, char *argv[]) {
    char *file_name = {"process_3.txt"};
    Queue list_of_processes = {NULL};
    Queue ready_queue = {NULL};

    if (argc < 2) {
        printf("Not enough arguments!\n");
        return 1;
    }

    int scheduler_type, time_quantum;
    if (strcmp(argv[1],"FCFS") == 0) {
        scheduler_type = 0;
        printf("Executing FCFS Scheduling Algorithm\n");
    }
    else if (strcmp(argv[1],"SJN") == 0) {
        scheduler_type = 1;
        printf("Executing SJN Scheduling Algorithm\n");
    }
    else if (strcmp(argv[1], "PRIORITY") == 0) {
        scheduler_type = 2;
        printf("Executing Priority Scheduling Algorithm\n");
    }
    else if (strcmp(argv[1], "RR") == 0 && argc == 4) {
        time_quantum = atoi(argv[3]);
        if (strcmp(argv[2], "FCFS") == 0) {
            scheduler_type = 3;
            printf("---- Running RR FCFS ON CPU ----\n");
        }
        else if (strcmp(argv[2], "SRTN") == 0) {
            scheduler_type = 4;
            printf("---- Running RR SRTN ON CPU ----\n");
        }
        else if (strcmp(argv[2], "PRIORITY") == 0) {
            scheduler_type = 5;
            printf("---- Running RR PRIORITY ON CPU ----\n");
        }
        else {
            printf("Error! Type of RR not specified, EXITING!"); 
            return 1;
        }
    }

    // Read in data from the processes.txt and store into queue
    interpret_data_from_text_file(file_name, &list_of_processes);
    printf("%d processes added to queue..\n", list_of_processes.current_size);
    current_queue(&ready_queue);

    // Create named pipe in temp folder on host computer
    char *link = "/tmp/kendall_project_link";
    int fd;

    // Establish link
    mkfifo(link, 0666);

    // If the scheduler_type >= 3, then send time quantum to cpu_emulator as well
    // Otherwise, do not do anything
    if (scheduler_type >= 3) {
        printf("TQ = %d\n", time_quantum);
        fd = open(link, O_WRONLY);
        write(fd, &time_quantum, sizeof(int));
        close(fd);
        usleep(200);
    }

    // Make buffer same size as block, 14 items that hold lengths of up to 100.
    char buffer[8][100]; 
    int clock = 0;
    int number_of_context_switches = 0;
    int process_exists, preempt = 0;
    while (process_exists != -1) {
        // Get current clock time
        fd = open(link, O_RDONLY);
        read(fd, &clock, sizeof(int));
        close(fd);
        usleep(500);

        // Create a pointer to the dequeued PCB
        PCB *p;
        printf("-------------\n");
        printf("IN SCHEDULER PROCESS\n");
        printf("Ran %d time(s):\n", clock);

        // For scheduling algorithms that are non-preemptive
        current_queue(&ready_queue);
        while (list_of_processes.current_size != 0 && list_of_processes.queue_size[0]->arrival <= clock) {
            // Dequeue the process in front of the queue from list_of_processes
            p = dequeue(&list_of_processes);
            list_of_processes.current_size--;
            ready_queue.current_size++;
            ready_queue.rear = ready_queue.current_size - 1;
            enqueue(&ready_queue, p);
        }

        // SJN
        if (scheduler_type == 1 || scheduler_type == 4) {
            runSJN(&ready_queue);
        }

        // Priority
        else if (scheduler_type == 2 || scheduler_type == 5) {
            runPriority(&ready_queue);
        }

        // Check if there is anything present in the ready_queue
        // If there is nothing present in the ready_queue, increment clock
        if (ready_queue.current_size != 0) {
            // Indicate to cpu that there is a process that exists.
            process_exists = 1;
            fd = open(link, O_WRONLY);
            write(fd, &process_exists, sizeof(int));
            close(fd);
            usleep(200);

            // Dequeue the process in front of the queue from ready_queue 
            p = dequeue(&ready_queue);

            // Copy from process block to buffer to send to named pipe
            memcpy(buffer, p->BLOCK, sizeof(p->BLOCK));

            // Open named pipe to send PCB that was dequeued to the CPU Emulator
            fd = open(link, O_WRONLY);
            write(fd, buffer, sizeof(buffer));
            close(fd);
            usleep(200);

            // If preemptive algorithm called
            if (scheduler_type >= 3) {
                int local, index = 0;
                while (1) {
                    if (list_of_processes.current_size != 0) {
                        /*if ((scheduler_type == 4 && (clock == list_of_processes.queue_size[0]->arrival 
                          && p->est_remaining_run_time - local >= list_of_processes.queue_size[0]->est_remaining_run_time))
                          || (scheduler_type != 4 || scheduler_type != 1 && clock == list_of_processes.queue_size[0]->arrival)) {*/
                        if (scheduler_type == 4 && clock == list_of_processes.queue_size[index]->arrival) {
                            if (p->est_remaining_run_time - local >= list_of_processes.queue_size[index]->est_remaining_run_time) {
                                preempt = 1;
                            }
                            else {
                                preempt = 0;
                                index++;
                            }
                        }
                        else if (scheduler_type == 5 && clock == list_of_processes.queue_size[index]->arrival) {
                            if (p->priority >= list_of_processes.queue_size[index]->priority) {
                                preempt = 1;
                            }
                            else {
                                preempt = 0;
                                index++;
                            }
                        }

                        if (preempt == 1) {
                            printf("PREEMPTED\n");
                            fd = open(link, O_WRONLY);
                            write(fd, &preempt, sizeof(int));
                            close(fd);
                            preempt = 0;
                            index = 0;
                            printf("---\n");
                            break; 
                        }
                    }
                    fd = open(link, O_WRONLY);
                    write(fd, &preempt, sizeof(int));
                    close(fd);
                    usleep(200);

                    fd = open(link, O_RDONLY);
                    read(fd, &local, sizeof(int));
                    close(fd);
                    usleep(1000);
                    if (local == time_quantum || local == p->est_remaining_run_time) break;

                    fd = open(link, O_RDONLY);
                    read(fd, &clock, sizeof(int));
                    close(fd);
                    usleep(200);
                    }
                }
                // Process in dealing with process is the same in both preemptive and non-preemptive
                // Receive updated PCB from the CPU Emulator
                printf("RECEVING\n");
                fd = open(link, O_RDONLY);
                read(fd, buffer, sizeof(buffer));
                close(fd);
                usleep(200);

                // Read file from buffer and store it into the BLOCK array
                memcpy(p->BLOCK, buffer, sizeof(p->BLOCK));
                p->est_remaining_run_time = strtol(p->BLOCK[7], NULL, 0);

                // If the remaining running time is not zero, we add it back into the queue
                if (scheduler_type >= 3) {
                    if (p->est_remaining_run_time > 0) {
                        printf("Process [%s] has returned to the queue with remaining: %d\n", p->process_name, p->est_remaining_run_time);
                        enqueue(&ready_queue, p);
                        number_of_context_switches++;
                    }
                    else {
                        printf("Process [%s] has finished and has been removed from the queue completely\n", p->process_name);
                        p = NULL;
                        ready_queue.current_size--;
                        number_of_context_switches++;
                    }
                }
                // Otherwise, remove it from the queue and decrement the queue size
                else {
                    printf("Process [%s] has finished and has been removed from the queue completely\n", p->process_name);
                    p = NULL;
                    ready_queue.current_size--;
                    number_of_context_switches++;
                }
            }
            // If no processes in ready queue, send a integer to indicate that there are no processes in ready_queue
            // Value indicated by -1
            else {
                printf("No process exists at time: %d\n", clock);
                process_exists = (list_of_processes.current_size == 0) ? -1 : 0;
                fd = open(link, O_WRONLY);
                write(fd, &process_exists, sizeof(int));
                close(fd);
                usleep(200);
            }
            printf("-------------\n");
            usleep(1000);
        }
        printf("Number of context switches: %d\n", number_of_context_switches);
        printf("Clock time: %ds\n", clock);
        return 0;

    }
