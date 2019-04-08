#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#define   LOCK_SH   1    /* shared lock */
#define   LOCK_EX   2    /* exclusive lock */
#define   LOCK_NB   4    /* don't block when locking */
#define   LOCK_UN   8    /* unlock */
#define BUFF_SIZE 40
#define MAX_SIZE 120

void sort_numbers(int arr[MAX_SIZE]) {

}

int main(int argc, char *argv[]) {
    FILE *read_file = fopen(argv[1], "r");
    if (read_file == NULL) {
        printf("Error, %s cannot be opened\n", argv[1]);
        exit(0);
    }

    FILE *main_file = fopen(argv[0], "a");
    if (main_file == NULL) {
        printf("Error, %s cannot be opened", argv[0]);
        exit(0);
    }

    char buffer[BUFF_SIZE];

    // If file is already locked, busy wait
    while (flock(fileno(main_file), LOCK_EX) == -1) {
        sleep(2); // Wait 2 s
    }
    
    if (flock(fileno(main_file), LOCK_EX) != -1) { // Lock released
        // Do work here
        if (fgets(buffer, BUFF_SIZE, read_file)) {

        }
    }


    return 0;
}
