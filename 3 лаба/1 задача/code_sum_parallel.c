#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE 10000000

int main(int argc, char** argv) {
    int rank, size;
    int* full_array = NULL;
    int* local_array = NULL;
    long long local_sum = 0, total_sum = 0;

    MPI_Init(&argc, &argv);                  // Инициализация MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    // Номер процесса
    MPI_Comm_size(MPI_COMM_WORLD, &size);    // Общее число процессов

    int chunk_size = ARRAY_SIZE / size;
    int remainder = ARRAY_SIZE % size;

    if (rank == 0) {
        full_array = (int*)malloc(ARRAY_SIZE * sizeof(int));
        if (!full_array) {
            fprintf(stderr, "Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for (int i = 0; i < ARRAY_SIZE; ++i) {
            full_array[i] = i + 1;
        }
    }

    local_array = (int*)malloc(chunk_size * sizeof(int));
    if (!local_array) {
        fprintf(stderr, "Memory allocation failed (rank %d)\n", rank);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Barrier(MPI_COMM_WORLD);  // Синхронизация перед началом замера времени
    double start_time = MPI_Wtime();

    // Распределение данных
    MPI_Scatter(full_array, chunk_size, MPI_INT, local_array, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Локальное суммирование
    for (int i = 0; i < chunk_size; ++i) {
        local_sum += local_array[i];
    }

    // Учёт "хвоста", если массив не делится нацело
    if (rank == 0 && remainder > 0) {
        for (int i = ARRAY_SIZE - remainder; i < ARRAY_SIZE; ++i) {
            local_sum += full_array[i];
        }
    }

    // Суммирование результатов от всех процессов
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        printf("%.6f\n", end_time - start_time);
        fflush(stdout);  // Обязателен для корректного вывода в HPC
    }

    free(local_array);
    if (rank == 0) free(full_array);

    MPI_Finalize();
    return 0;
}