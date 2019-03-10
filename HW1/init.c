#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {

    char *execute_cpu_emu[] = {"gcc", "cpu_emulator.c", "-o", "execute_cpu_emulator", NULL};
    char *done_one[] = {"./execute_cpu_emulator"};
    char *done_two[] = {"./execute_scheduling_process"};
    char *execute_scheduling_process[] = {"gcc", "scheduler_process.c", "-o", "execute_scheduling_process", NULL};


    int status;
    int compile_programs_0 , compile_programs_1, execute_program_1;
    int execute_program_0 = fork();

    if (execute_program_0 > 0) {
        printf("Forking Another Child...");
        compile_programs_0 = fork();
        if (compile_programs_0 == 0) {
            compile_programs_1 = fork();
            if (compile_programs_1 == 0) {
                printf("Compiling emulator...\n");
                execv("/usr/bin/gcc", execute_cpu_emu);
            }
            else {
                wait(NULL);
                printf("Compiling scheduler...\n");
                execv("/usr/bin/gcc", execute_scheduling_process);
            }
        }
        else {
            waitpid(compile_programs_0, &status, 0);
            execute_program_1 = fork();
            if (execute_program_0 == 0) {
                printf("Executing emulator...\n");
                execv("./execute_cpu_emulator", done_one);
            }
            else {
                wait(NULL);
                execv("./execute_scheduling_process", done_two);

            }
        }
    }
    else  {
        waitpid(execute_program_0, &status, 0);
        waitpid(compile_programs_0, &status, 0);
    }

    return 0;
}
