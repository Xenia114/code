#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void merge(int* array, int left, int mid, int right, int* temp) {
    int i = left, j = mid + 1, k = left;

    while (i <= mid && j <= right) {
        if (array[i] <= array[j]) temp[k++] = array[i++];
        else temp[k++] = array[j++];
    }
    while (i <= mid) temp[k++] = array[i++];
    while (j <= right) temp[k++] = array[j++];

    for (int idx = left; idx <= right; ++idx) {
        array[idx] = temp[idx];
    }
}

void merge_sort(int* array, int left, int right, int* temp) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        merge_sort(array, left, mid, temp);
        merge_sort(array, mid + 1, right, temp);
        merge(array, left, mid, right, temp);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <array_size>\n", argv[0]);
        return 1;
    }

    int ARRAY_SIZE = atoi(argv[1]);
    if (ARRAY_SIZE <= 0) {
        fprintf(stderr, "Неверный размер массива\n");
        return 1;
    }

    int* array = (int*)malloc(ARRAY_SIZE * sizeof(int));
    int* temp = (int*)malloc(ARRAY_SIZE * sizeof(int));
    if (!array || !temp) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        free(array);
        free(temp);
        return 1;
    }

    srand((unsigned)time(NULL));
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        array[i] = rand() % 100;
    }

    clock_t start = clock();
    merge_sort(array, 0, ARRAY_SIZE - 1, temp);
    clock_t end = clock();

    double time_taken = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("Merge sort time: %f seconds\n", time_taken);

    free(array);
    free(temp);

    return 0;
}