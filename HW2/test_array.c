#include <stdio.h>
#include <stdlib.h>

void affect_array(int *arr, int size) {
    for (int i = 0; i < size; i++) {
        arr[i] = 22;
    }
}

int main() {
    int arr[] = {5,3,2,1};
    int size = sizeof(arr)/sizeof(arr[0]);

    affect_array(arr, size);

    for (int i = 0; i < size; i++) {
        printf("%d\n", arr[i]);
    }
}
