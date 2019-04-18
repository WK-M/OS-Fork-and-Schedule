#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/file.h>
#define LOCK_EX   2    /* exclusive lock */
#define LOCK_NB   4    /* don't block when locking */
#define LOCK_UN   8    /* unlock */

int count_lines(FILE *to_be_read_file);
void sort_values(int number_list[], int total);
int *combine_arrays(int *to_be_merged, int merge_total, int *data, int data_total, int *arr, int new_size);

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

/* @param:
 * to_be_read_file - file name to be read
 * 
 * return:
 * number of lines in file
*/
int count_lines(FILE *to_be_read_file) {
    int ch;
    int lines = 0;

    while ((ch = getc(to_be_read_file)) != EOF) {
        if (ch == '\n') lines++;
    }

    return lines;
}

/* @param: 
 * to_be_merged - newx.dat
 * data - datafile.dat
 * merge_total - amount of numbers in newx.dat
 * data_total - amount of numbers datafile.dat
 * arr - sorted array with numbers from newx.dat and datafile.dat
 * new_size - total amount
 *
 * return:
 * array with sorted values
*/
int *combine_arrays(int *to_be_merged, int merge_total, int *data, int data_total, int *arr, int new_size) {
    /* Loop counter
     * i - for merged
     * j - for main data file
     * counter - current index
    */
    int counter = 0, i = 0, j = 0; 
    while (i < merge_total && j < data_total) {
        if (to_be_merged[i] < data[j]) {
            arr[counter] = to_be_merged[i];
            i++;
        }
        else if (to_be_merged[i] > data[j]) {
            arr[counter] = data[j];
            j++;
        }
        else {
            arr[counter] = to_be_merged[i];
            i++;
        }
        counter++;
    }

    if (i == merge_total) {
        for (int k = j; k < data_total; k++){
            arr[counter] = data[k];
            counter++;
        }
    }
    else {
        for (int k = i; k < merge_total; k++) {
            arr[counter] = to_be_merged[k];
            counter++;
        }
    }
    return arr;
}

int main(int argc, char *argv[]) {

    // Create pointer for file to continuously check if file is not locked
    FILE *main_data;
    while (flock(fileno(main_data = fopen(argv[1], "r+")), LOCK_EX | LOCK_NB) == -1) {
        int count = 0;
        if (main_data == NULL) {
            printf("Error, %s cannot be opened\n", argv[1]);
            exit(0);
        }
        while (count != 4500) count++;
    }

    printf("PID: %d, GRABBED LOCK\n", getpid());

    // newx.dat
    FILE *main_file = fopen(argv[2], "r");
    if (main_file == NULL) {
        printf("Error, %s cannot be opened", argv[2]);
        exit(0);
    }
    // ------------------- Critical Region ------------------- //

    printf("PID: %d, Entering Critical Region...\n", getpid());
    // Read number of lines in newx.dat and then move the pointer back to the beginning using rewind
    int size_of_content0 = count_lines(main_file);
    rewind(main_file);

    // Create array with specific size
    int read_content0[size_of_content0];
    // Indexing Variable
    int counter0 = 0;
    // Add numbers from newx.dat into array and then close the newx.dat file
    while (fscanf(main_file, "%d", &read_content0[counter0]) != EOF) {
        counter0++;
    }
    fclose(main_file);

    // Sort the array
    sort_values(read_content0, counter0);

    // Read number of lines datafile.dat and then move the pointer back to the beginning using rewind
    int size_of_content1 = count_lines(main_data);
    rewind(main_data);

    // Create array with specific size
    int read_content1[size_of_content1];

    // Indexing Variable
    int counter1 = 0;
    // Add the total count in the datafile.dat to the number of items in the main_file
    while (fscanf(main_data, "%d", &read_content1[counter1]) != EOF) {
        counter1++;
    }

    // After adding all the items into the array, and given a total number of items we have into our array.
    // Merge the two arrays together to get a new array
    int total = counter0 + counter1;
    int arr[total];
    int *merge_array = combine_arrays(read_content0, counter0, read_content1, counter1, arr, total);

    // Now let's write it back to the main file
    // Open file again and write array to the file
    main_data = fopen(argv[1], "r+");
    for (int i = 0; i < total; i++) {
        fprintf(main_data, "%d\n", arr[i]);
    }
    // fclose here not only closes the file descriptor, but it also releases the lock as well
    fclose(main_data);
    printf("PID: %d, Exiting Critical Region\n", getpid());
    //flock(fileno(main_data), LOCK_UN | LOCK_NB);
    printf("PID: %d, File unlocked\n\n", getpid());
    return 0;
}
