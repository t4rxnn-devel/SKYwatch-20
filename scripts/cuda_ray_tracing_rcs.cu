#include <cuda_runtime.h>
#include <stdio.h>

__global__ void calculate_panel_rcs_kernel(const float* normals, const float* areas, 
                                           float* rcs_out, float3 wave_vector, 
                                           int num_panels) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < num_panels) {
        float3 norm = make_float3(normals[idx*3], normals[idx*3+1], normals[idx*3+2]);
        float dot_prod = norm.x * wave_vector.x + norm.y * wave_vector.y + norm.z * wave_vector.z;
        
        // Physical optics scattering amplitude per panel
        if (dot_prod < 0.0f) { // Facing radar wave
            float cos_theta = -dot_prod;
            rcs_out[idx] = 4.0f * 3.14159265f * (areas[idx] * areas[idx]) * (cos_theta * cos_theta);
        } else {
            rcs_out[idx] = 0.0f;
        }
    }
}

extern "C" void launch_rcs_kernel(const float* h_normals, const float* h_areas, 
                                 float* h_rcs, int num_panels) {
    float *d_normals, *d_areas, *d_rcs;
    size_t vec_size = num_panels * 3 * sizeof(float);
    size_t scalar_size = num_panels * sizeof(float);

    cudaMalloc((void**)&d_normals, vec_size);
    cudaMalloc((void**)&d_areas, scalar_size);
    cudaMalloc((void**)&d_rcs, scalar_size);

    cudaMemcpy(d_normals, h_normals, vec_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_areas, h_areas, scalar_size, cudaMemcpyHostToDevice);

    float3 wave_vec = make_float3(0.0f, 0.0f, -1.0f);
    int threadsPerBlock = 256;
    int blocksPerGrid = (num_panels + threadsPerBlock - 1) / threadsPerBlock;

    calculate_panel_rcs_kernel<<<blocksPerGrid, threadsPerBlock>>>(d_normals, d_areas, d_rcs, wave_vec, num_panels);

    cudaMemcpy(h_rcs, d_rcs, scalar_size, cudaMemcpyDeviceToHost);

    cudaFree(d_normals);
    cudaFree(d_areas);
    cudaFree(d_rcs);
}
