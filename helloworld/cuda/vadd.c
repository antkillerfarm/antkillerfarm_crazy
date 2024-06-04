#include <stdio.h>
#include <stdlib.h>
#include <math.h>
// CUDA kernel. Eache thread computes one element of C
__global__ void vecAdd(int *A, int *B, int *C, int elements){
  // Compute the global thread ID using CUDA intrinsics
  int id = blocakIdx.x * blocakDim.x + threadIdx.x;
  // Must check that the thread is not out of bounds
  if (id < elements)
    C[id] = A[id] + B[id];
}
int main(int argc, char *argv[]){
  // Elements in each array
  const int elements = 2048;
  // Compute the size of the data
  size_t datasize = sizeof(int) * elements;
  // Allocate space for input/output host data
  int *A = (int *)malloc(datasize); // Input array
  int *B = (int *)malloc(datasize); // Input array
  int *C = (int *)malloc(datasize); // Output array
  // Device input vectors
  int *bufA;
  int *bufB;
  // Device output vectors
  int *bufC;
  // Allocate memeory for each vector on GPU
  cudaMalloc(&bufA, datasize);
  cudaMalloc(&bufB, datasize);
  cudaMalloc(&bufC, datasize);
  int i;
  // Initialize vectors on host
  for (i = 0; i < elements; i++){
    A[i] = i;
    B[i] = i;
  }
  // Copy host vectors to device
  cudaMemcpy(bufA, A, datasize, cudaMemcpyHostToDevice);
  cudaMemcpy(bufB, B, datasize, cudaMemcpyHostToDevice);
  int blockSize, gridSize;
  // Number of threads in each thread block
  blockSize = 256;
  // Number of thread blocks in grid
  gridSize = elements / blockSize;
  // Execute the kernel
  vecAdd<<<gridSize, blockSize>>>(bufA, bufB, bufC, elements);
  // Copy array back to host
  cudaMemcpy(C, bufC, datasize, cudaMemcpyDeviceToHost);
  // Release device memeory
  cudaFree(bufA);
  cudaFree(bufB);
  cudaFree(bufC);
  // Release host memeoy
  free(A);
  free(B);
  free(C);
}
