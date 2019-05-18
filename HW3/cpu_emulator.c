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

int RR(int time_quantum, int est_remaining_run_time) {
    while (time_quantum-- != 0) {
        est_remaining_run_time--;
        if (est_remaining_run_time == 0)
            return 0;
    }
    return est_remaining_run_time;
}

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
    // fd1: file descriptor for named pipe
    int time_quantum, process_exists, fd1;
    char *link = "/tmp/kendall_project_link";

    // Establish link
    mkfifo(link, 0666);

    int clock = 0;
    fd1 = open(link, O_WRONLY);
    write(fd1, &clock, sizeof(int));
    close(fd1);
    usleep(500);

    /*fd1 = open(link, O_RDONLY);
    read(fd1, &time_quantum, sizeof(int));
    close(fd1);
    usleep(500);
    printf("---- Time Quantum Retreived! TQ = %d ----\n", time_quantum);*/

    char RECEIVED_PCB_INFO[8][100];
    while (1) {
        fd1 = open(link, O_RDONLY);
        read(fd1, &process_exists, sizeof(int));
        close(fd1);
        usleep(500);

        // If ready queue has no processes in it
        if (process_exists == 0) {
            // Update clock
            clock++;
            fd1 = open(link, O_WRONLY);
            write(fd1, &clock, sizeof(int));
            close(fd1);
            usleep(500);
        }

        // list_of_processes queue is completely empty
        else if (process_exists == -1) {
            printf("All processes dequeued from the txt file... EXITING\n");
            return 0;
        }

        // If a process is available on the ready queue
        else {
            printf("PCB RECEIVED\n");
            // Open up the named pipe and read from it
            fd1 = open(link, O_RDONLY);
            read(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
            close(fd1);

            printf("-------------\n");
            printf("IN CPU EMULATOR:\n");
            printf("\nRan %d time(s):\n", clock++);

            int retreived_time = strtol(RECEIVED_PCB_INFO[7], NULL, 10);
            printf("Process: [%s], Current Run time is: [%d]\n", RECEIVED_PCB_INFO[1], retreived_time);

            // Update time after running on CPU
            if (scheduler_type >= 3) {
                int updated_run_time = RR(time_quantum, retreived_time);
                sprintf(RECEIVED_PCB_INFO[7], "%d", updated_run_time);
            }
            else {
                while (--retreived_time != 0) clock++;
                sprintf(RECEIVED_PCB_INFO[7], "%d", retreived_time);
            }

            // Send the PCB back to the scheduler to handle
            fd1 = open(link, O_WRONLY);
            write(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
            printf("Process: [%s], New Run time is: [%s]\n", RECEIVED_PCB_INFO[1],RECEIVED_PCB_INFO[7]);
            printf("-------------\n");
            close(fd1);
            usleep(500); // Sleep to allow scheduler to receive data

            // Account for context switch
            clock+=2;
            fd1 = open(link, O_WRONLY);
            write(fd1, &clock, sizeof(clock));
            printf("CLOCK TIME %d sent to SCHEDULER\n", clock);
            close(fd1);
            usleep(500);
        }
    }
}
