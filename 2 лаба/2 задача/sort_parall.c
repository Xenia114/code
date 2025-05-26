#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

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

        #pragma omp task if (last_index - first_index > 10000)
        QuickSort(array, first_index, j);

        #pragma omp task if (last_index - first_index > 10000)
        QuickSort(array, i, last_index);

        #pragma omp taskwait
    }
}

void RunParallelQuicksort(int num_threads) {
    struct timespec start, end;
    double time_taken;
    int* array = malloc(ARRAY_SIZE * sizeof(int));
    int* sorted_array = malloc(ARRAY_SIZE * sizeof(int));

    if (!array || !sorted_array) {
        printf("Ошибка выделения памяти\n");
        return;
    }

    srand(time(NULL));
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        array[i] = rand() % 100;
    }

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        sorted_array[i] = array[i];
    }

    omp_set_num_threads(num_threads);
    clock_gettime(CLOCK_REALTIME, &start);
    #pragma omp parallel
    {
        #pragma omp single nowait
        QuickSort(sorted_array, 0, ARRAY_SIZE - 1);
    }
    clock_gettime(CLOCK_REALTIME, &end);

    time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("time: %f\n", time_taken); // Только время без текста
    fflush(stdout);

    free(array);
    free(sorted_array);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Использование: %s <количество_потоков>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    RunParallelQuicksort(num_threads);
    return 0;
}
