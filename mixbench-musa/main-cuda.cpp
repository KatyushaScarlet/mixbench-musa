/**
 * main-cuda.cpp: This file is part of the mixbench GPU micro-benchmark suite.
 *
 * Contact: Elias Konstantinidis <ekondis@gmail.com>
 **/

#include <stdio.h>
#include <stdlib.h>
#include <musa.h>
#include <musa_runtime.h>
#include <string.h>
#include "lcutil.h"
#include "mix_kernels_cuda.h"
#include "version_info.h"

#define VECTOR_SIZE (32 * 1024 * 1024)

int main(int argc, char* argv[]) {
    printf("mixbench (%s)\n", VERSION_INFO);

    unsigned int datasize = VECTOR_SIZE * sizeof(double);

    musaSetDevice(0);
    StoreDeviceInfo(stdout);

    size_t freeCUDAMem, totalCUDAMem;
    musaMemGetInfo(&freeCUDAMem, &totalCUDAMem);
    printf("Total GPU memory %lu, free %lu\n", totalCUDAMem, freeCUDAMem);
    printf("Buffer size:          %dMB\n", datasize / (1024 * 1024));

    double* c;
    c = (double*)malloc(datasize);

    mixbenchGPU(c, VECTOR_SIZE);

    free(c);

    return 0;
}
