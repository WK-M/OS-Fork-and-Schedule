#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    int cpu_emu = fork();

    if (cpu_emu > 0) {
        int scheduler = fork();
        if (scheduler != 0) {
            char *execute_cpu_emu[] = {"gcc", "cpu_emulator.c", "-o", "./execute_cpu_emulator", NULL};
            execv("./execute_cpu_emulator", execute_cpu_emu);
        }
        else {
            wait(NULL);
            char *execute_scheduling_process[] = {"gcc", "scheduler_process.c", "-o", "./execute_scheduling_process", NULL};
            execv("./execute_scheduling_process", execute_scheduling_process);
        }
    }

    else 
        printf("Init intiialized\n");

    return 0;
}
