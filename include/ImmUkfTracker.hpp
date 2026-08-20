// SkyWatch-20 Interacting Multiple Model Unscented Kalman Filter (IMM-UKF)
// Industry-Standard Aerospace Target Tracking Core Module

#pragma once
#include <vector>
#include <string>

struct TrackingState {
    // 3D Kinematics State Vectors: [X, Y, Z, Velocity_X, Velocity_Y, Velocity_Z]
    double state_vector[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    double covariance_matrix[6][6];
};

class ImmUkfTracker {
private:
    std::string m_target_callsign;
    double m_model_probabilities[3]; // [Constant Velocity, Constant Acceleration, Coordinated Turn]
    double m_transition_matrix[3][3];
    
    // Unscented Transform Sigma Parameter metrics
    int m_num_sigma_points;
    double m_lambda;

    void mix_filter_states() noexcept;
    void predict_sigma_points(double dt) noexcept;
    void update_measurement_vectors(const double raw_radar_reading[3]) noexcept;

public:
    ImmUkfTracker(std::string callsign) noexcept;
    
    // Primary pipeline hook executed on every radar antenna sweep revolution
    void process_radar_sweep(const double measurement_xyz[3], double dt_seconds) noexcept;
    
    [[nodiscard]] TrackingState get_refined_state() const noexcept;
};
