// CUDA 12.1

// System includes
#include <stdio.h>
#include <assert.h>

// CUDA runtime
#include <cuda_runtime.h>

// helper functions and utilities to work with CUDA
// #include <helper_functions.h>
//#include <helper_cuda.h>
/*
__global__
void saxpy(int n, float a, float *x, float *y)
{
  int i = blockIdx.x*blockDim.x + threadIdx.x;
  if (i < n) y[i] = a*x[i] + y[i];
}

*/
__global__
void insanely_long_division(int N, int *out, int *x, int *y)
{


    for(int i=0;i<N;i++){

        printf("[%d, %d]: ",\
            blockIdx.y*gridDim.x+blockIdx.x,\
            threadIdx.z*blockDim.x*blockDim.y+threadIdx.y*blockDim.x+threadIdx.x);

        int count = 0;
        int top = x[i];
        int bottom = y[i];

        for (int j=bottom;j<top;j++){

            if (top % j == 0){
                count += 1;
            } else {
                continue;
            }

        }

        out[i] = count;

        printf("%d\n",count);
    }


}




int main(int argc, char **argv)
{

    int devID;
    cudaDeviceProp props;

    // This will pick the best possible CUDA capable device
//    devID = findCudaDevice(argc, (const char **)argv);

    //Get GPU information
//    checkCudaErrors(cudaGetDevice(&devID));
//    checkCudaErrors(cudaGetDeviceProperties(&props, devID));
    cudaGetDevice(&devID);
    cudaGetDeviceProperties(&props, devID);
    printf("Device %d: \"%s\" with Compute %d.%d capability\n",
            devID, props.name, props.major, props.minor);

    printf("printf() is called. Output:\n\n");

    int N = 100;

    int A = 0;

    int *res, *x, *y, *d_res, *d_x, *d_y;

    res = (int*)malloc(N*sizeof(int));
    x = (int*)malloc(N*sizeof(int));
    y = (int*)malloc(N*sizeof(int));

    cudaMalloc(&d_res, N*sizeof(int)); 
    cudaMalloc(&d_x, N*sizeof(int)); 
    cudaMalloc(&d_y, N*sizeof(int));

    for (int i = 0; i < N; i++) {
        res[i] = 0;
        x[i] = 100;
        y[i] = 5;
    }

    cudaMemcpy(d_res, res, N*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_x, x, N*sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(d_y, y, N*sizeof(int), cudaMemcpyHostToDevice);


    //  saxpy<<<(N+255)/256, 256>>>(N, 2.0f, d_x, d_y);

    printf("counting total insane division..........\n");

    insanely_long_division<<<N, 1>>>(N, d_res, d_x, d_y);

    cudaDeviceSynchronize();

    cudaMemcpy(res, d_res, N*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(x, d_x, N*sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(y, d_y, N*sizeof(int), cudaMemcpyDeviceToHost);

    for (int i = 0; i < N; i++) {
        A += res[i];
    
    }

    printf("\ntotal insane division count: %d\n", A);
    printf("\ncompleted!\n");

    cudaFree(d_res);
    cudaFree(d_x);
    cudaFree(d_y);
    free(res);
    free(x);
    free(y);



    return EXIT_SUCCESS;
}
