#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define ARRAY_SIZE 200000

// Простая пузырьковая сортировка
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

// Объединение и частичная сортировка слиянием двух отсортированных массивов
void MergeAndSplit(int* local, int* neighbor, int local_size, int keep_small) {
    int* combined = (int*)malloc(local_size * 2 * sizeof(int));
    for (int i = 0; i < local_size; i++) {
        combined[i] = local[i];
        combined[i + local_size] = neighbor[i];
    }

    BubbleSort(combined, local_size * 2);

    if (keep_small) {
        for (int i = 0; i < local_size; i++)
            local[i] = combined[i];
    }
    else {
        for (int i = 0; i < local_size; i++)
            local[i] = combined[i + local_size];
    }

    free(combined);
}

int main(int argc, char** argv) {
    int rank, size;
    int* data = NULL;
    int local_size;
    int* local_array;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    local_size = ARRAY_SIZE / size;
    local_array = (int*)malloc(local_size * sizeof(int));

    if (rank == 0) {
        data = (int*)malloc(ARRAY_SIZE * sizeof(int));
        srand(time(NULL));
        for (int i = 0; i < ARRAY_SIZE; ++i)
            data[i] = rand() % 100;
    }

    MPI_Scatter(data, local_size, MPI_INT, local_array, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    // Локальная сортировка
    BubbleSort(local_array, local_size);

    // Odd-Even Transposition
    for (int phase = 0; phase < size; phase++) {
        int partner = -1;
        if (phase % 2 == 0) { // Четная фаза
            if (rank % 2 == 0) partner = rank + 1;
            else partner = rank - 1;
        }
        else { // Нечетная фаза
            if (rank % 2 == 0) partner = rank - 1;
            else partner = rank + 1;
        }

        if (partner >= 0 && partner < size) {
            int* neighbor = (int*)malloc(local_size * sizeof(int));
            MPI_Sendrecv(local_array, local_size, MPI_INT, partner, 0,
                neighbor, local_size, MPI_INT, partner, 0,
                MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (rank < partner)
                MergeAndSplit(local_array, neighbor, local_size, 1);
            else
                MergeAndSplit(local_array, neighbor, local_size, 0);

            free(neighbor);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    double end_time = MPI_Wtime();

    // Собираем отсортированные блоки обратно
    MPI_Gather(local_array, local_size, MPI_INT, data, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("MPI Odd-Even Transposition Sort completed in %f seconds\n", end_time - start_time);
        // Опционально: проверьте сортировку или выведите часть массива
    }

    free(local_array);
    if (rank == 0) free(data);

    MPI_Finalize();
    return 0;
}