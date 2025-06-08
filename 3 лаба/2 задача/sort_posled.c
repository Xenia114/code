#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int ARRAY_SIZE = 200000;

void BubbleSort(int* array, int size) {
    int temp;
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

void CopyArray(int* source, int* target) {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        target[i] = source[i];
    }
}

int main() {
    clock_t start, end;
    double time_taken;

    int* array = (int*)malloc(ARRAY_SIZE * sizeof(int));
    int* sorted_array = (int*)malloc(ARRAY_SIZE * sizeof(int));

    if (array == NULL || sorted_array == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return 1;
    }

    srand((unsigned)time(NULL));
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        array[i] = rand() % 100;
    }

    CopyArray(array, sorted_array);

    start = clock();
    BubbleSort(sorted_array, ARRAY_SIZE);
    end = clock();

    time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Bubble sort time: %f seconds\n", time_taken);

    free(array);
    free(sorted_array);

    return 0;
}
