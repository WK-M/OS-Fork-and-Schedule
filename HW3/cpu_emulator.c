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

int update_PCB_run_time(int time_quantum, int est_remaining_run_time) {
    return est_remaining_run_time - time_quantum;
}

int main(int argc, char *argv[]) {
    // time_quantum: value used to run a process on CPU and send it back to the scheduler
    int time_quantum = 200, fd1, scheduler_type;

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

    char *link = "/tmp/kendall_project_link";

    // Establish link
    mkfifo(link, 0666);

    char RECEIVED_PCB_INFO[8][100];
    int clock = 0;
    while (1) {
        // Open up the named pipe and read from it
        fd1 = open(link, O_RDONLY);
        read(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));

        // If null PCB received, close pipe and exit
        if (strlen(RECEIVED_PCB_INFO[1]) == 0) {
            close(fd1);
            printf("CPU EMULATOR ENDING...\n");
            return 0;
        }
        printf("-------------\n");
        printf("IN CPU EMULATOR:\n");
        printf("\nRan %d time(s):\n", clock++);

        int retreived_time = strtol(RECEIVED_PCB_INFO[7], NULL, 10);
        printf("Process: [%s], Current Run time is: [%d]\n", RECEIVED_PCB_INFO[1], retreived_time);

        // Update time after running on CPU
        /*int updated_run_time = update_PCB_run_time(time_quantum, retreived_time);
        sprintf(RECEIVED_PCB_INFO[7], "%d", updated_run_time);*/
        if (scheduler_type >= 3) {

        }
        else {
            while (--retreived_time != 0) clock++;
            sprintf(RECEIVED_PCB_INFO[7], "%d", retreived_time);
        }
        close(fd1);

        // Send the PCB back to the scheduler to handle
        fd1 = open(link, O_WRONLY);
        write(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
        printf("Process: [%s], New Run time is: [%s]\n", RECEIVED_PCB_INFO[1],RECEIVED_PCB_INFO[7]);
        printf("-------------\n");
        close(fd1);
        sleep(1);

        clock+=2;
        fd1 = open(link, O_WRONLY);
        write(fd1, &clock, sizeof(clock));
        printf("SENT %d to SCHEDULER\n", clock);
        close(fd1);
    }
}
