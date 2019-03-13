#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

    char *done_one[] = {"./cpu_emulator", NULL};
    char *done_two[] = {"./scheduler_process", NULL};


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
        waitpid(execute_program_0, &status, 0);
        waitpid(execute_program_1, &status, 0);
    }

    return 0;
}

