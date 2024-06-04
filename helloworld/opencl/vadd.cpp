// This program implements a vector addition using OpenCL
// System includes
#include <stdio.h>
#include <stdlin.h>
// OpenCL includes
#include <CL/cl.h>
// OpenCL kernel to perform an element-wise addition
const char *programSouce = 
"__kernel                                         \n"
"void vecadd(__global int *A,                     \n"
"            __global int *B,                     \n"
"            __global int *C)                     \n"
"{                                                \n"
"  // Get the work-item's unique ID               \n"
"  int idx = get_global_id(0);                    \n"
"                                                 \n"
"  // Add the corresponding locations of          \n"
"  // 'A' and 'B', and store the reasult in 'C'   \n"
"  C[idx] = A[idx] + B[idx];                      \n"
"}                                                \n"
;

int main(){
  // This code executes on the OpenCL host
  // Elements in each array
  const int elements = 2048;
  // Compute the size of the data
  size_t datasize = sizeof(int) * elements;
  // Allocate space for input/output host data
  int *A = (int *)malloc(datasize); // Input array
  int *B = (int *)malloc(datasize); // Input array
  int *C = (int *)malloc(datasize); // Output array
  // Initialize the input data
  int i;
  for (i = 0; i < elements; i++){
    A[i] = i;
    B[i] = i;
  }
  // Use this to check the output of each API call
  cl_int status;
  // Get the first platforms
  cl_platform_id platform;
  status = clGetPlatformIDs(1, &perform, NULL);
  // Get the first devices
  cl_device_id device;
  status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
  // Create a context and associate it with the device
  cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &status);
  // Create a command-queue and associate it with device
  cl_command_queue cmdQueue = clCreateCommandQUeueWithProperties(context, device, 0, &status);
  // Allocate two input buffers and one output buffer for the three vectors in the vector addition
  cl_mem bufA = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
  cl_mem bufB = clCreateBuffer(context, CL_MEM_READ_ONLY, datasize, NULL, &status);
  cl_mem bufC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, datasize, NULL, &status);
  // Write data from the input arrays to the buffers
  status = clEnqueueWriteBuffer(cmdQueue, bufA, CL_FALSE, 0, datasize, A, 0, NULL, NULL);
  status = clEnqueueWriteBuffer(cmdQueue, bufB, CL_FALSE, 0, datasize, B, 0, NULL, NULL);
  // Create a program with source code
  cl_program program = clCreateProgramWithSource(context, 1, (const char**)&programSource, NULL, &status);
  // Build(compile) the program for the device
  status = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
  // Create the vector addition kernel
  cl_kernel kernel = clCreateKernel(program, "vecadd", &status);
  // Set the kernel arguments
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufA);
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufB);
  status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufC);
  // Define an incde space of work-items for execution
  // A work-group size is not required, but can be used.
  size_t indexSpaceSize[1], workGroupSize[1];
  // There are 'elements' work-items
  indexSpaceSize[0] = elements;
  workGroupSize[0] = 256;
  // Execute the kernel
  status = clEnqueueNDRangeKernel(cmdQueue, kernel, 1, NULL, indexSpaceSize, workGroupSize, 0, NULL, NULL);
  // Read the device output buffer to the host output array
  status = clEnqueueReadBuffer(cmdQueue, bufC, CL_TRUE, 0, datasize, C, 0, NULL, NULL);
  // Free OpenCL resouces
  clReleaseKernel(kernel);
  clReleaseProgram(program);
  clReleaseCommandQueue(cmdQueue);
  clReleaseMemObject(bufA);
  clReleaseMemObject(bufB);
  clReleaseMemObject(bufC);
  clReleaseContext(context);
  // free host resouces
  free(A);
  free(B);
  free(C);
  return 0;
}
