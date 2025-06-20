#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <time.h>

#define ARRAY_SIZE 10000000
#define THREADS_PER_BLOCK 256

__global__ void sum_reduction(int* input, long long* partial_sums, int n) {
    extern __shared__ int sdata[];

    unsigned int tid = threadIdx.x;
    unsigned int i = blockIdx.x * blockDim.x + threadIdx.x;

    sdata[tid] = (i < n) ? input[i] : 0;
    __syncthreads();

    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            sdata[tid] += sdata[tid + s];
        }
        __syncthreads();
    }

    if (tid == 0) {
        partial_sums[blockIdx.x] = sdata[0];
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <num_runs> <threads_per_block>\n", argv[0]);
        return 1;
    }

    int runs = atoi(argv[1]);
    int threads_per_block = atoi(argv[2]);

    int* h_array = (int*)malloc(ARRAY_SIZE * sizeof(int));
    if (!h_array) {
        fprintf(stderr, "Host memory allocation failed!\n");
        return 1;
    }

    for (int i = 0; i < ARRAY_SIZE; ++i) {
        h_array[i] = i + 1;
    }

    int* d_array;
    long long* d_partial_sums;
    cudaMalloc((void**)&d_array, ARRAY_SIZE * sizeof(int));
    cudaMemcpy(d_array, h_array, ARRAY_SIZE * sizeof(int), cudaMemcpyHostToDevice);

    int blocks = (ARRAY_SIZE + threads_per_block - 1) / threads_per_block;
    cudaMalloc((void**)&d_partial_sums, blocks * sizeof(long long));

    long long total_time = 0;

    for (int run = 0; run < runs; ++run) {
        cudaDeviceSynchronize();
        clock_t start = clock();

        sum_reduction<<<blocks, threads_per_block, threads_per_block * sizeof(int)>>>(d_array, d_partial_sums, ARRAY_SIZE);
        cudaDeviceSynchronize();

        long long* h_partial_sums = (long long*)malloc(blocks * sizeof(long long));
        cudaMemcpy(h_partial_sums, d_partial_sums, blocks * sizeof(long long), cudaMemcpyDeviceToHost);

        long long total_sum = 0;
        for (int i = 0; i < blocks; ++i)
            total_sum += h_partial_sums[i];

        clock_t end = clock();
        total_time += (end - start);

        free(h_partial_sums);
    }

    double avg_time = (double)total_time / runs / CLOCKS_PER_SEC;
    printf("%.6f\n", avg_time);  // Для вывода в скрипт

    free(h_array);
    cudaFree(d_array);
    cudaFree(d_partial_sums);

    return 0;
}
