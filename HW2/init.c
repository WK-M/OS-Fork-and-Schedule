#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>

int main() {

    // Needed for execv
    char *p1[] = {"./add_files", "datafile.dat", "new1.dat", NULL};
    char *p2[] = {"./add_files", "datafile.dat", "new2.dat", NULL};
    char *p3[] = {"./add_files", "datafile.dat", "new3.dat", NULL};

    int status;
    int execute_program_2, execute_program_3;
    int execute_program_1 = fork();

    if (execute_program_1 == 0) {
        execv(p1[0], p1);
    }
    else {
        execute_program_2 = fork();
        if (execute_program_2 == 0) {
            execv(p2[0], p2);
        }
        else {
            execute_program_3 = fork();
            if (execute_program_3 == 0) {
                execv(p3[0], p3);
            }
        }
    }
    wait(NULL);

    return 0;

}
