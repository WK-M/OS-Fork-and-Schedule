#include <stdio.h>

int *combine_arrays(int *to_be_merged, int merge_total, int *data, int data_total, int *arr, int new_size) {
    // i - for merged
    // j - for main data file
    int counter = 0, i = 0, j = 0; // Loop counter

    while (i < merge_total && j < data_total) {
        printf("T\n");
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
        printf("j\n");
        for (int k = j; k < data_total; k++){
            arr[counter] = data[k];
            counter++;
        }
    }

    else {
        printf("J\n");
        for (int k = i; k < merge_total; k++) {
            arr[counter] = to_be_merged[k];
            counter++;
        }
    }

    return arr;
}

int main() {
    int arr1[] = {1,5,9,19};
    int arr2[] = {0,2,3,9,13};

    int arr1_size = sizeof(arr1)/sizeof(arr1[0]);
    int arr2_size = sizeof(arr2)/sizeof(arr2[0]);

    int arr[arr1_size+arr2_size];
    int *new_arr = combine_arrays(arr2, arr2_size, arr1, arr1_size, arr, arr1_size+arr2_size);
    
    for (int i = 0; i < arr1_size+arr2_size; i++) {
        printf("%d: %d\n", i, new_arr[i]);
    }
}
