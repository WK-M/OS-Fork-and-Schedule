#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#define   LOCK_SH   1    /* shared lock */
#define   LOCK_EX   2    /* exclusive lock */
#define   LOCK_NB   4    /* don't block when locking */
#define   LOCK_UN   8    /* unlock */
#define BUFF_SIZE 40
#define MAX_SIZE 120

int append_items(char *number_list, FILE *to_be_read_file, int counter);

/* @param: 
 * buffer: array with values read from a previous text file
 * counter: current index
 *
 * return:
 * updated counter loop
 */

int append_items(char *number_list, FILE *to_be_read_file, int counter) {
    int num;
    char newline = getc(to_be_read_file);
    while (newline != EOF) { // check if its not end of file
        // Continue to build number as new digit is found
        // Once newline is equal to a newline, exit the loop
        while (newline != '\n') {
            if (newline > '9' || newline < '0') newline = getc(to_be_read_file);  
            num *= 10;
            num += newline - '0';
        }
        // Add new number to array
        number_list[counter] = num;
        counter++; // update number of lines in datafile.dat

        // Move on to the next 
        newline = getc(to_be_read_file);
    }
    return counter;
}

int main(int argc, char *argv[]) {
    // datafile.dat
    FILE *read_file = fopen(argv[1], "r");
    if (read_file == NULL) {
        printf("Error, %s cannot be opened\n", argv[1]);
        exit(0);
    }

    // newx.dat
    FILE *main_file = fopen(argv[0], "a");
    if (main_file == NULL) {
        printf("Error, %s cannot be opened", argv[0]);
        exit(0);
    }

    char buffer[BUFF_SIZE];
    char read_content[MAX_SIZE];

    // If file is already locked, busy wait
    while (flock(fileno(main_file), LOCK_EX) == -1) {
        sleep(2); // Wait 2 s
    }
    
    if (flock(fileno(main_file), LOCK_EX) != -1) { // Lock released, try to obtain lock
        // Do work here
        int size = sizeof(read_content)/sizeof(read_content[0]);
        int count = 0; // Holds current number of items read
        count = append_items(read_content, read_file, count);
        count = append_items(read_content, main_file, count);



        // find number of lines in file
        /*int counter = 0, num;
        char newline = getc(read_file);
        while (newline != EOF) { // check if its not end of file
            // Continue to build number as new digit is found
            // Once newline is equal to a newline, exit the loop
            while (newline != '\n') {
                if (newline > '9' || newline < '0') newline = getc(read_file);  
                num *= 10;
                num += newline - '0';
            }
            // Add new number to array
            read_content[counter] = num;
            counter++; // update number of lines in datafile.dat

            // Move on to the next c
            newline = getc(read_file);
        }

        // Append newly read numbers into array
        // start new index
        while (newline != EOF) {

        }*/
        //

    }
    return 0;
}

