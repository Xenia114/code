#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int ARRAY_SIZE = 1000000;

void QuickSort(int* array, int first_index, int last_index) {
    if (first_index < last_index) {
        int pivot = array[(first_index + last_index) / 2];
        int i = first_index;
        int j = last_index;
        int temp;

        while (i <= j) {
            while (array[i] < pivot) i++;
            while (array[j] > pivot) j--;
            if (i <= j) {
                temp = array[i];
                array[i] = array[j];
                array[j] = temp;
                i++;
                j--;
            }
        }

        QuickSort(array, first_index, j);
        QuickSort(array, i, last_index);
    }
}

void CopyArray(int* source, int* target) {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        target[i] = source[i];
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <количество_потоков>\n", argv[0]);
        return 1;
    }

    struct timespec start, end;
    double time_taken;

    int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));
    int *sorted_array = (int *)malloc(ARRAY_SIZE * sizeof(int));

    if (array == NULL || sorted_array == NULL) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        return 1;
    }

    srand(time(0));
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        array[i] = rand() % 100;
    }

    CopyArray(array, sorted_array);

    clock_gettime(CLOCK_REALTIME, &start);
    QuickSort(sorted_array, 0, ARRAY_SIZE - 1);
    clock_gettime(CLOCK_REALTIME, &end);

    time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("time: %f\n", time_taken);

    free(array);
    free(sorted_array);

    return 0;
}
