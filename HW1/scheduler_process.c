/*
 * Kendall Molas
 * CSc 33200
 * Project 1
 * Professor Gordon
 */
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 

#define NUMBER_OF_PCBS 7
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
    char process_name[5];
    int pid;
    char state[5];
    int priority;
    int is_interruptable;
    int est_total_run_time;
    int est_remaining_run_time;

    // Ask if this is ok
    char BLOCK[7][100];
} PCB;

typedef struct queue {
    PCB *queue_size[NUMBER_OF_PCBS];
    int front;
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
            strcpy(p->process_name, process_pointer);
            
            process_pointer = strtok(NULL, ",");
            if (process_pointer != NULL) {
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
            strcpy(p->BLOCK[0], p->process_name);
            sprintf(p->BLOCK[1], "%d", p->pid);
            strcpy(p->BLOCK[2], p->state);
            sprintf(p->BLOCK[3], "%d", p->priority);
            sprintf(p->BLOCK[4], "%d", p->is_interruptable);
            sprintf(p->BLOCK[5], "%d", p->est_total_run_time);
            sprintf(p->BLOCK[6], "%d", p->est_remaining_run_time);

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

// Remove PCB from ready queue
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

int main() {
    char *file_name = {"processes.txt"};
    Queue queue = {NULL};
    interpret_data_from_text_file(file_name, &queue);
    printf("%d processes added to queue..\n", queue.current_size);

    char *link = "/tmp/kendall_project_link";
    int fd;

    mkfifo(link, 0666);

    // Make buffer same size as block, 7 items that hold lengths of up to 100.
    char buffer[7][100]; 
    int counter = 1;

    while (queue.current_size != 0) {
        // Create a pointer to the dequeued PCB
        printf("Ran %d time(s):\n", counter);
        PCB *p = dequeue(&queue);
        // Copy from process block to buffer to send to named pipe
        memcpy(buffer, p->BLOCK, sizeof(p->BLOCK));
        current_queue(&queue);
        //printf("%s\n", p->BLOCK[6]);
        fd = open(link, O_WRONLY);
        write(fd, buffer, sizeof(buffer));
        close(fd);

        printf("\n");

        fd = open(link, O_RDONLY);
        read(fd, buffer, sizeof(buffer));
        memcpy(p->BLOCK, buffer, sizeof(p->BLOCK));
        p->est_remaining_run_time = strtol(p->BLOCK[6], NULL, 0);

        if (p->est_remaining_run_time != 0) {
            enqueue(&queue, p);
        }
        else {
            printf("Process [%s] has finished and has been removed from the queue completely\n", p->process_name);
            p = NULL;
            queue.current_size--;
        }
        printf("\n");
        current_queue(&queue);
        close(fd);
        printf("-------------\n");
        counter++;
    }

    memset(buffer, 0, sizeof(buffer));
    fd = open(link, O_WRONLY);
    write(fd, buffer, sizeof(buffer));
    close(fd);
    printf("SCHEDULER ENDING...");
    return 0;
}
