#include <stdio.h>
#include <stdlib.h>
int main() {
    int number_list[] = {5, 2, 3, 1, 8, 20, 14, 33, 8};
    int total = sizeof(number_list) / sizeof(number_list[0]);
    for (int i = 0; i < total; i++) {
        int min = number_list[i];
        printf("Loop %d, min: %d\n", i, min);
        for (int j = i; j < total; j++) {
            if (min > number_list[j]) {
                min = number_list[j];
                int temp = number_list[i];
                number_list[i] = min;
                number_list[j] = temp;
            }
        }
        for (int k = 0; k < total; k++) {
            printf("%d\n", number_list[k]);
        }
        printf("---------------\n");
    }

}
