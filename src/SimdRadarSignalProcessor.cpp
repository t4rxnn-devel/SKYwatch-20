#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>

namespace skywatch::dsp {

using ComplexSample = std::complex<float>;

class SimdRadarSignalProcessor {
private:
    float noise_floor_db_;

public:
    explicit SimdRadarSignalProcessor(float noise_floor_db = -90.0f) 
        : noise_floor_db_(noise_floor_db) {}

    // SIMD-accelerated Complex Magnitude & Phase Thresholding Filter
    void apply_clutter_suppression(const std::vector<ComplexSample>& raw_iq_in, 
                                   std::vector<float>& filtered_power_out) {
        size_t n = raw_iq_in.size();
        filtered_power_out.resize(n);

        const ComplexSample* in_ptr = raw_iq_in.data();
        float* out_ptr = filtered_power_out.data();

        // Vectorized loop hint for compiler SIMD optimization
        #pragma omp simd
        for (size_t i = 0; i < n; ++i) {
            float real = in_ptr[i].real();
            float imag = in_ptr[i].imag();
            float mag_sq = (real * real) + (imag * imag);
            
            // Convert to dB scale
            float db_val = 10.0f * std::log10(mag_sq + 1e-12f);
            
            // MTI (Moving Target Indicator) Thresholding
            out_ptr[i] = (db_val > noise_floor_db_) ? mag_sq : 0.0f;
        }
    }
};

} // namespace skywatch::dsp

int main() {
    skywatch::dsp::SimdRadarSignalProcessor processor(-80.0f);
    std::vector<std::complex<float>> mock_iq(1024, {0.001f, 0.002f});
    mock_iq[512] = {1.5f, 0.8f}; // Target plot spike
    
    std::vector<float> power_spectrum;
    processor.apply_clutter_suppression(mock_iq, power_spectrum);
    
    std::cout << "[SIMD DSP Core] Filtered Signal Peak at Index 512: " << power_spectrum[512] << std::endl;
    return 0;
}
