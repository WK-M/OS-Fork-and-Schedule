#include <stdio.h> 
#include <string.h> 
#include <stdlib.h>
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 

int update_PCB_run_time(int time_quantum, int est_remaining_run_time) {
    printf("%d\n", est_remaining_run_time - time_quantum);
    if (est_remaining_run_time - time_quantum > 0)
        return est_remaining_run_time - time_quantum;
    else
        return 0;
}

int main() {
    int time_quantum = 80, fd1;

    char *link = "/tmp/kendall_project_link";

    mkfifo(link, 0666);

    char RECEIVED_PCB_INFO[7][100];
    int counter = 1;
    while (1) {
        fd1 = open(link, O_RDONLY);
        read(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
        printf("Ran %d time(s):\n", counter);

        int current_time = strtol(RECEIVED_PCB_INFO[6], NULL, 10);
        printf("Process: %s, Current Run time is: %d\n", RECEIVED_PCB_INFO[0], current_time);
        int updated_run_time = update_PCB_run_time(time_quantum, current_time);
        sprintf(RECEIVED_PCB_INFO[6], "%d", updated_run_time);
        close(fd1);

        fd1 = open(link, O_WRONLY);
        write(fd1, RECEIVED_PCB_INFO, sizeof(RECEIVED_PCB_INFO));
        printf("Process: %s, New Run time is: %s\n", RECEIVED_PCB_INFO[0],RECEIVED_PCB_INFO[6]);
        printf("----------------\n");
        close(fd1);
        counter++;
    }

    return 0;
}

