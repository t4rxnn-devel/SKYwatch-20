// ============================================================================
// SkyWatch-20 Interacting Multiple Model Unscented Kalman Filter Header
// Compliant with EUROCONTROL Surveillance Target Tracking Frameworks
// ============================================================================

#pragma once
#include <string>
#include <vector>

// Fixed dimensions matching our 6D kinematic state array profiles
constexpr int STATE_DIM = 6;
constexpr int MEASURE_DIM = 3;
constexpr int SIGMA_COUNT = 2 * STATE_DIM + 1;

struct TrackingState {
    double state_vector[STATE_DIM];
    double covariance_matrix[STATE_DIM][STATE_DIM];
};

class ImmUkfTracker {
private:
    std::string m_target_callsign;
    double m_mu[3];
    double m_p_ij[3][3];
    
    double m_state_x[STATE_DIM];
    double m_cov_P[STATE_DIM][STATE_DIM];

    double m_alpha;
    double m_kappa;
    double m_beta;
    double m_lambda;
    double m_weights_c[SIGMA_COUNT];
    double m_weights_m[SIGMA_COUNT];

    void calculate_ut_weights() noexcept;
    void mix_filter_states() noexcept;
    void predict_sigma_points(double dt) noexcept;
    void update_measurement_vectors(double meas_x, double meas_y, double meas_z) noexcept;

public:
    explicit ImmUkfTracker(std::string callsign) noexcept;
    
    // Master antenna sweep pipeline tracker execution block
    void process_radar_sweep(double raw_x, double raw_y, double raw_z, double dt_seconds) noexcept;
    
    [[nodiscard]] TrackingState get_refined_state() const noexcept;
};

// Pipeline C-Linkage Interface declaration hook
extern "C" {
    void process_tracker_step(const char* callsign, double x, double y, double z, double dt);
}
