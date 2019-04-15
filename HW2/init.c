#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#define LOCK_SH   1    /* shared lock */
#define LOCK_EX   2    /* exclusive lock */
#define LOCK_NB   4    /* don't block when locking */
#define LOCK_UN   8    /* unlock */

int main() {
    char *p1[] = {"./add_files", "datafile.dat", "new1.dat", NULL};
    char *p2[] = {"./add_files", "datafile.dat", "new2.dat", NULL};
    char *p3[] = {"./add_files", "datafile.dat", "new3.dat", NULL};

    int execute_program_2, execute_program_3;
    int execute_program_1 = fork();

    if (execute_program_1 == 0) {
        sleep(3);
        execute_program_2 = fork();
        if (execute_program_2 == 0) {
            execute_program_3 = fork();
            if (execute_program_3 == 0) {
                execv(p3[0], p3);
            }
            else {
                execv(p2[0], p2);
            }
        }
        else {
            execv(p1[0], p1);
        }
    }
    else {
        wait(NULL);
    }

    return 0;

}
