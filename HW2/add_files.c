#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#define LOCK_SH   1    /* shared lock */
#define LOCK_EX   2    /* exclusive lock */
#define LOCK_NB   4    /* don't block when locking */
#define LOCK_UN   8    /* unlock */
#define MAX_SIZE 120

int read_numbers(int *number_list, FILE *to_be_read_file);
void sort_values(int number_list[], int total);
void *combine_arrays(int *to_be_merged, int *data, int total);

/* @param: 
 * buffer: array with values read from a previous text file
 * counter: current index
 *
 * return:
 * updated counter loop
 */

// Implementation of Insertion Sort
void sort_values(int *number_list, int total) {
    for (int i = 0; i < total; i++) {
        int min = number_list[i];
        for (int j = i; j < total; j++) {
            if (min > number_list[j]) {
                min = number_list[j];
                int temp = number_list[i];
                number_list[i] = min;
                number_list[j] = temp;
            }
        }
    }
}

int read_numbers(int *number_list, FILE *to_be_read_file) {
    int counter = 0;;
    while (fscanf(to_be_read_file, "%ld", &number_list[counter]) != EOF) {
        counter++;
    }
    return counter;
}

void *combine_arrays(int *to_be_merged, int *data, int total) {
    int arr[total];
    int counter = 0, i = 0, j = 0; // Loop counter
    int merge_total = sizeof(to_be_merged)/sizeof(to_be_merged[0]);
    int data_total = sizeof(data)/sizeof(data[0]);

    // i > j
    if (merge_total < data_total) {
        while (i < merge_total) {
            if (to_be_merged[i] < data[j]) {
                arr[counter] = to_be_merged[i];
                i++;
                counter++;
            }
            else {
                arr[counter] = data[j];
                j++;
                counter++;
            }
        }


    }

    // i < j
    else if (merge_total > data_total) {
        while (j < data_total) {
            arr[counter] = 
        }

    }

    // i == j
    else {

    }


}

int main(int argc, char *argv[]) {

    // newx.dat
    FILE *main_file = fopen(argv[2], "wb");
    if (main_file == NULL) {
        printf("Error, %s cannot be opened", argv[2]);
        exit(0);
    }

    int read_content0[MAX_SIZE];
    int read_content1[MAX_SIZE];

    // If file is already locked, busy wait
    while (flock(fileno(main_file), LOCK_EX) == -1) {
        sleep(2); // Wait 2 s
    }
    
    if (flock(fileno(main_file), LOCK_EX) != -1) { // Lock released, try to obtain lock
        // Do work here
        // Add numbers from newx.dat into array
        // And get the total number of items in newx.dat
        int count = 0;
        count = read_numbers(read_content0, main_file);
        // Sort the array
        sort_values(read_content0, count);

        fclose(main_file);

        // datafile.dat
        FILE *read_file = fopen(argv[1], "r");
        if (read_file == NULL) {
            printf("Error, %s cannot be opened\n", argv[1]);
            exit(0);
        }

        // Add the total count in the datafile.dat to the number of items in the main_file
        count += read_numbers(read_content1, read_file);

        // After adding all the items into the array, and given a total number of items we have into our array.
        // Merge the two arrays together to get a new array
        int arr[count] = combine_arrays(read_content0, read_content1, count);

        // Now let's write it back to the main file
        for (int i = 0; i < count; i++) {
            fwrite(arr, sizeof(int), sizeof(arr), read_file);
        }
        fclose(read_file);
    }
    return 0;
}

