/*
 * Kendall Molas
 * CSc 33200
 * Project 1
 * Professor Gordon
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

    // Commands to execute file
    char *done_one[] = {"./cpu_emulator", NULL};
    char *done_two[] = {"./scheduler", NULL};


    int status;
    int execute_program_1;
    int execute_program_0 = fork();
    if (execute_program_0 == 0) {
        execute_program_1 = fork();
        if (execute_program_1 == 0) {
            execv(done_two[0], done_two);
        }
        else {
            execv(done_one[0], done_one);
        }
    }
    else  {
        // Wait for both programs to finish and then kill parent process
        waitpid(execute_program_0, &status, 0);
        waitpid(execute_program_1, &status, 0);
    }

    printf("Both programs finished running, exiting init...\n");

    return 0;
}

