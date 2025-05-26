#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ARRAY_SIZE 10000000

void fill_array_sequential(int *arr) {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        arr[i] = i + 1;
    }
}

long long sequential_sum(int *arr) {
    long long sum = 0;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        sum += arr[i];
    }
    return sum;
}

int main() {
    int *array = (int *)malloc(ARRAY_SIZE * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    fill_array_sequential(array);

    clock_t start_time = clock();
    long long result = sequential_sum(array);
    clock_t end_time = clock();

    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    // Только время — для скрипта
    printf("%.6f\n", time_taken);

    free(array);
    return 0;
}