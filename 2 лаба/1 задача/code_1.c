#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define ARRAY_SIZE 10000000

void fill_array_sequential(int *arr) {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        arr[i] = i + 1;
    }
}

long long parallel_sum(int *arr, int num_threads) {
    long long sum = 0;
    #pragma omp parallel for num_threads(num_threads) reduction(+:sum)
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        sum += arr[i];
    }
    return sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[1]);
    if (num_threads <= 0) {
        fprintf(stderr, "Number of threads must be positive\n");
        return 1;
    }

    int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    fill_array_sequential(array);

    double start_time = omp_get_wtime();
    long long result = parallel_sum(array, num_threads);
    double time_taken = omp_get_wtime() - start_time;

    // Выводим только время в формате, который будет парсить скрипт
    printf("%.6f\n", time_taken);

    free(array);
    return 0;
}