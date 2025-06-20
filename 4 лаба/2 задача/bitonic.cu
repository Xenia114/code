#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <time.h>

__global__ void bitonicSortStep(int* data, int j, int k, int size) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= size) return;

    int ixj = i ^ j;
    if (ixj > i) {
        if (((i & k) == 0 && data[i] > data[ixj]) ||
            ((i & k) != 0 && data[i] < data[ixj])) {
            int temp = data[i];
            data[i] = data[ixj];
            data[ixj] = temp;
        }
    }
}

void bitonicSort(int* data, int size, int threads) {
    int* dev_data;
    cudaMalloc((void**)&dev_data, size * sizeof(int));
    cudaMemcpy(dev_data, data, size * sizeof(int), cudaMemcpyHostToDevice);

    int blocks = (size + threads - 1) / threads;

    for (int k = 2; k <= size; k <<= 1) {
        for (int j = k >> 1; j > 0; j >>= 1) {
            bitonicSortStep<<<blocks, threads>>>(dev_data, j, k, size);
            cudaDeviceSynchronize();
        }
    }

    cudaMemcpy(data, dev_data, size * sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(dev_data);
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <ARRAY_SIZE> <THREADS_PER_BLOCK>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    int threads = atoi(argv[2]);

    // Проверка: размер должен быть степенью двойки
    if ((size & (size - 1)) != 0) {
        fprintf(stderr, "Error: size must be a power of 2\n");
        return 1;
    }

    int* data = (int*)malloc(size * sizeof(int));
    if (!data) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    srand((unsigned)time(NULL));
    for (int i = 0; i < size; ++i) {
        data[i] = rand() % 10000;
    }

    cudaEvent_t start, stop;
    float milliseconds = 0;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start, 0);

    bitonicSort(data, size, threads);

    cudaEventRecord(stop, 0);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&milliseconds, start, stop);

    printf("%.6f\n", milliseconds / 1000.0f);

    free(data);
    return 0;
}
