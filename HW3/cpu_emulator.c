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

int main(int argc, char *argv[]) {
    int scheduler_type;
    if (strcmp(argv[1],"FCFS") == 0) {
        scheduler_type = 0;
        printf("---- RUNNING FCFS ON CPU ----\n");
    }
    else if (strcmp(argv[1],"SJN") == 0) {
        scheduler_type = 1;
        printf("---- RUNNING SJN ON CPU ----\n");
    }
    else if (strcmp(argv[1],"PRIORITY") == 0) {
        scheduler_type = 2;
        printf("---- RUNNING PRIORITY ON CPU ----\n");
    }
    else if (strcmp(argv[1], "RR") == 0) {
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
    else {
        printf("CPU Scheduling Algorithm Not Specified or Not Specified! EXITING!");
        return 1;
    }

    // time_quantum: value used to run a process on CPU and send it back to the scheduler
    // fd: file descriptor for named pipe
    int preempt, time_quantum, process_exists, fd;
    char *link = "/tmp/kendall_project_link";

    // Establish link
    mkfifo(link, 0666);

    if (scheduler_type >= 3) {
        fd = open(link, O_RDONLY);
        read(fd, &time_quantum, sizeof(int));
        close(fd);
        printf("---- Time Quantum Retreived! TQ = %d ----\n", time_quantum);
    }

    int local = 0, clock = 0;
    fd = open(link, O_WRONLY);
    write(fd, &clock, sizeof(int));
    close(fd);
    usleep(200);

    char RECEIVED_PCB_INFO[8][100];
    while (1) {
        fd = open(link, O_RDONLY);
        read(fd, &process_exists, sizeof(int));
        close(fd);
        usleep(500);

        // If ready queue has no processes in it
        if (process_exists == 0) {
            // Update clock
            clock++;
            fd = open(link, O_WRONLY);
            write(fd, &clock, sizeof(int));
            close(fd);
            usleep(200);
        }

        // list_of_processes queue is completely empty
        else if (process_exists == -1) {
            printf("All processes dequeued from the txt file... EXITING\n");
            return 0;
        }

        // If a process is available on the ready queue
        else {
            // Open up the named pipe and read from it
            printf("PCB RECEIVED\n");
            fd = open(link, O_RDONLY);
            read(fd, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
            close(fd);

            printf("-------------\n");
            printf("IN CPU EMULATOR:\n");
            printf("\nRan %d time(s):\n", clock);

            int retreived_time = strtol(RECEIVED_PCB_INFO[7], NULL, 10);
            printf("Process: [%s], Current Run time is: [%d]\n", RECEIVED_PCB_INFO[1], retreived_time);
            usleep(500);

            // Update time after running on CPU
            if (scheduler_type >= 3) {
                // PCB received and running on CPU, check if there is a process who just arrived
                // As long as process has not been preempted and that the local clock has not ran for the time_quantum
                // Continue checking if a process has arrived
                while (1) {
                    // Check if preempted
                    fd = open(link, O_RDONLY);
                    read(fd, &preempt, sizeof(int));
                    close(fd);
                    usleep(200);

                    if (preempt == 1) break;

                    // Send local clock to the scheduler
                    local++; // Update local clock
                    fd = open(link, O_WRONLY);
                    write(fd, &local, sizeof(int));
                    close(fd);
                    usleep(1500);
                    if (local == time_quantum || local == retreived_time) break;

                    // Update global clock to send back to scheduler
                    clock++;
                    fd = open(link, O_WRONLY);
                    write(fd, &clock, sizeof(int));
                    close(fd);
                    usleep(500);
                } 
                // If process has been preempted or the local time_quantum has been acheived, then updated PCB info
                if (preempt == 1 || local == time_quantum || local == retreived_time) {
                    printf("Current Clock: %d\n", clock);
                    int updated_run_time = (retreived_time - local < 0) ? 0 : retreived_time - local;
                    sprintf(RECEIVED_PCB_INFO[7], "%d", updated_run_time);
                    local = 0;
                    printf("Updated PCB\n");
                }
            }
            else {
                while (--retreived_time > 0) clock++;
                sprintf(RECEIVED_PCB_INFO[7], "%d", retreived_time);
                printf("Current Clock: %d\n", clock);
            }
            if (scheduler_type <= 2) {
                fd = open(link, O_WRONLY);
                write(fd, &clock, sizeof(int));
                close(fd);
                usleep(300);
            }
            // Send the PCB back to the scheduler to handle
            printf("Sending PCB\n");
            fd = open(link, O_WRONLY);
            write(fd, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
            printf("Process: [%s], New Run time is: [%s]\n", RECEIVED_PCB_INFO[1],RECEIVED_PCB_INFO[7]);
            printf("-------------\n");
            close(fd);
            usleep(200); // Sleep to allow scheduler to receive data

            // Account for context switch
            clock += 2;
            fd = open(link, O_WRONLY);
            write(fd, &clock, sizeof(clock));
            printf("CLOCK TIME %d sent to SCHEDULER\n", clock);
            close(fd);
            usleep(1000);
        }
    }
}
