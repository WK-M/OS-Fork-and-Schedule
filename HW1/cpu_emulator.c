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

int update_PCB_run_time(int time_quantum, int est_remaining_run_time) {
    return est_remaining_run_time - time_quantum;
}

int main() {
    // time_quantum: value used to run a process on CPU and send it back to the scheduler
    int time_quantum = 200, fd1;

    char *link = "/tmp/kendall_project_link";

    // Establish link
    mkfifo(link, 0666);

    char RECEIVED_PCB_INFO[7][100];
    int counter = 1;
    while (1) {
        // Open up the named pipe and read from it
        fd1 = open(link, O_RDONLY);
        read(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));

        // If null PCB received, close pipe and exit
        if (strlen(RECEIVED_PCB_INFO[0]) == 0) {
            close(fd1);
            printf("CPU EMULATOR ENDING...\n");
            return 0;
        }
        printf("-------------\n");
        printf("IN CPU EMULATOR:\n");
        printf("\nRan %d time(s):\n", counter);

        int current_time = strtol(RECEIVED_PCB_INFO[6], NULL, 10);
        printf("Process: [%s], Current Run time is: [%d]\n", RECEIVED_PCB_INFO[0], current_time);

        // Update time after running on CPU
        int updated_run_time = update_PCB_run_time(time_quantum, current_time);
        sprintf(RECEIVED_PCB_INFO[6], "%d", updated_run_time);
        close(fd1);

        // Send the PCB back to the scheduler to handle
        fd1 = open(link, O_WRONLY);
        write(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
        printf("Process: [%s], New Run time is: [%s]\n", RECEIVED_PCB_INFO[0],RECEIVED_PCB_INFO[6]);
        printf("-------------\n");
        close(fd1);
        counter++;
    }
}

