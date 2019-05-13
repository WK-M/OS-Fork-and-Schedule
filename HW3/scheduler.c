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
    char *type_of_scheduler[2];
    PCB *queue_size[NUMBER_OF_PCBS];
    unsigned int front;
    int rear;
    unsigned int current_size;
} Queue;


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

    int counter = 0;
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
            printf("PCB %d: [%s] %d %s %d %d %d %d\n", i, q->queue_size[i]->process_name, q->queue_size[i]->pid, 
                    q->queue_size[i]->state, q->queue_size[i]->priority, q->queue_size[i]->is_interruptable, 
                    q->queue_size[i]->est_total_run_time,q->queue_size[i]->est_remaining_run_time);
    }
}

// Add PCB into ready queue
void enqueue(Queue *q, PCB *p) {
    if (p->est_remaining_run_time > 0) {
        printf("Process [%s] has returned to queue\n", p->process_name);
        q->queue_size[q->rear] = p;
    }
}

void runFCFS(Queue *q) {
    // First specify the type of scheduler algorithm from argv
    q->type_of_scheduler[0] = "FCFS";
    PCB *min_process;
    // Sort queue based on arrival time
    for (int i = 0; i < q->current_size; i++) {
        min_process = q->queue_size[i];
        for (int j = i; j < q->current_size; j++) {
            if (min_process->arrival > q->queue_size[j]->arrival) {
                min_process = q->queue_size[j];
                PCB *temp_process = q->queue_size[i];
                q->queue_size[i] = min_process;
                q->queue_size[j] = temp_process;
            }
        }
    }
}

void runSJN(Queue *q) {
    // First specify the type of scheduler algorithm from argv
    q->type_of_scheduler[0] = "SJN";
    PCB *min_process;
    // Sort queue based on remaining run time
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

void runPriority(Queue *q) {
    // First specify the type of scheduler algorithm from argv
    q->type_of_scheduler[0] = "PRIORITY";
    PCB *process_with_low_priority;
    // Sort queue based on remaining run time
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
    printf("\nProcess [%s]: dequeued\n", dequeued_process->process_name);

    return dequeued_process;
}

int main(int argc, char *argv[]) {
    char *file_name = {"process_3.txt"};
    Queue ready_queue = {NULL};

    if (argc < 2) {
        printf("Not enough arguments!\n");
        return 1;
    }

    int scheduler_type;

    // Read in data from the processes.txt and store into queue
    interpret_data_from_text_file(file_name, &ready_queue);
    printf("%d processes added to queue..\n", ready_queue.current_size);

    if (strcmp(argv[1],"FCFS") == 0) {
        scheduler_type = 0;
        printf("Executing FCFS Scheduling Algorithm\n");
        ready_queue.type_of_scheduler[0] = "FCFS";
        runFCFS(&ready_queue);
    }

    else if (strcmp(argv[1],"SJN") == 0) {
        scheduler_type = 1;
        printf("Executing SJN Scheduling Algorithm\n");
        ready_queue.type_of_scheduler[0] = "SJN";
        runSJN(&ready_queue);
    }

    else if (strcmp(argv[1], "PRIORITY") == 0) {
        scheduler_type = 2;
        printf("Executing Priority Scheduling Algorithm\n");
        ready_queue.type_of_scheduler[0] = "Priority";
        runPriority(&ready_queue);
    }

    else if (strcmp(argv[1], "RR") == 0) {
        if (strcmp(argv[2], "FCFS") == 0) {

        }

        else if (strcmp(argv[2], "SRTN") == 0) {

        }
        
        else if (strcmp(argv[2], "PRIORITY") == 0) {

        }
    }

    else {
        printf("NOT A VALID SCHEDULING ALGORITHM\n");
        return 1;
    }
    
    current_queue(&ready_queue);

    char *link = "/tmp/kendall_project_link";
    int fd;

    // Establish link
    mkfifo(link, 0666);

    // Make buffer same size as block, 7 items that hold lengths of up to 100.
    char buffer[8][100]; 
    int counter = 1;
    int number_of_context_switches = 0;

    while (ready_queue.current_size != 0) {
        printf("-------------\n");
        printf("IN SCHEDULER PROCESS\n");
        printf("Ran %d time(s):\n", counter);

        // Create a pointer to the dequeued PCB
        PCB *p = dequeue(&ready_queue);
        // Copy from process block to buffer to send to named pipe
        memcpy(buffer, p->BLOCK, sizeof(p->BLOCK));
        current_queue(&ready_queue);

        // Open named pipe to send PCB that was dequeued to the CPU Emulator
        fd = open(link, O_WRONLY);
        write(fd, buffer, sizeof(buffer));
        close(fd);

        // Receive updated PCB from the CPU Emulator
        fd = open(link, O_RDONLY);
        read(fd, buffer, sizeof(buffer));

        // Read file from buffer and store it into the BLOCK array
        memcpy(p->BLOCK, buffer, sizeof(p->BLOCK));
        p->est_remaining_run_time = strtol(p->BLOCK[7], NULL, 0);

        // If the remaining running time is not zero, we add it back into the queue
        if (type_of_scheduler >= 3) {
            if (p->est_remaining_run_time > 0) {
                enqueue(&ready_queue, p);
            }
            else {

            }
        }
        // Otherwise, remove it from the queue and decrement the queue size
        else {
            printf("Process [%s] has finished and has been removed from the queue completely\n", p->process_name);
            p = NULL;
            ready_queue.current_size--;
            number_of_context_switches++;
        }

        close(fd);
        printf("-------------\n");
        current_queue(&ready_queue);
        sleep(1);

        fd = open(link, O_RDONLY);
        read(fd, &counter, sizeof(int));
        close(fd);
    }
    // Send a NULL message to the CPU Emulator to dictate that queue is empty
    memset(buffer, 0, sizeof(buffer));
    fd = open(link, O_WRONLY);
    write(fd, buffer, sizeof(buffer));
    close(fd);
    printf("SCHEDULER ENDING...\n");

    printf("Number of context switches: %d\n", number_of_context_switches);
    return 0;
}

