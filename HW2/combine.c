#include <stdio.h>

int *combine_arrays(int *to_be_merged, int *data, int total) {
    printf("t\n");
    int arr[total];
    // i - for merged
    // j - for main data file
    int counter = 0, i = 0, j = 0; // Loop counter
    int merge_total = sizeof(to_be_merged)/sizeof(to_be_merged[0]);
    int data_total = sizeof(data)/sizeof(data[0]);

    // i > j
    if (merge_total > data_total) {
        while (i < merge_total) {
            printf("T\n");
            if ((to_be_merged[i] < data[j]) && j < data_total){
                arr[counter] = to_be_merged[i];
                i++;
            }
            else if ((to_be_merged[i] >= data[j]) && j < data_total) {
                arr[counter] = data[j];
                j++;
            }
            else {
                arr[counter] = to_be_merged[i];
                i++;
            }
            counter++;
        }
    }
    // i < j
    else if (merge_total < data_total) {
        while (j < data_total) {
            printf("E\n");
            if ((to_be_merged[i] < data[j]) && i < merge_total) {
                arr[counter] = to_be_merged[i];
                i++;
            }
            else if ((to_be_merged[i] > data[j]) && i < merge_total) {
                arr[counter] = data[j];
                j++;
            }
            else {
                arr[counter] = to_be_merged[j];
                j++;
            }
            counter++;
        }
    }
    // i == j
    else {
        while (data_total == merge_total && counter <= data_total+merge_total) {
            if (to_be_merged[i] < data[j]) {
                arr[counter] = to_be_merged[i];
                i++;
            }
            else {
                arr[counter] = data[j];
                j++;
            }
            counter++;
        }
    }

    return arr;
}

int main() {
    int arr1[] = {1,5,9,13,15};
    int arr2[] = {0,2,3,9,13};

    int *new_arr = combine_arrays(arr2, arr1, 10);
    
    for (int i = 0; i < 10; i++) { 
        printf("%d\n", new_arr[i]);
    }
}
