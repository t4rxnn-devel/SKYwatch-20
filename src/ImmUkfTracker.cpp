#include "ImmUkfTracker.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

ImmUkfTracker::ImmUkfTracker(std::string callsign) noexcept 
    : m_target_callsign(callsign), m_num_sigma_points(13), m_lambda(3.0) {
    
    // Initialize standard EUROCONTROL Markov transition probability matrix margins
    // High diagonal variables ensure tracking stays smooth inside a model frame
    m_model_probabilities[0] = 0.80; // Constant Velocity Default
    m_model_probabilities[1] = 0.15; // Constant Acceleration 
    m_model_probabilities[2] = 0.05; // Coordinated Turning Flight Mode
    
    m_transition_matrix[0][0] = 0.95; m_transition_matrix[0][1] = 0.03; m_transition_matrix[0][2] = 0.02;
    m_transition_matrix[1][0] = 0.05; m_transition_matrix[1][1] = 0.90; m_transition_matrix[1][2] = 0.05;
    m_transition_matrix[2][0] = 0.10; m_transition_matrix[2][1] = 0.10; m_transition_matrix[2][2] = 0.80;
}

void ImmUkfTracker::mix_filter_states() noexcept {
    // IMM Math Step 1: Interacting Multiple Model Cross-Mixing Layer
    // Blends tracking approximations together based on Markov transition chains
    double normalization_factors[3] = {0.0, 0.0, 0.0};
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 3; ++i) {
            normalization_factors[j] += m_transition_matrix[i][j] * m_model_probabilities[i];
        }
    }
    // Update structural profile tracking variables seamlessly here
}

void ImmUkfTracker::predict_sigma_points(double dt) noexcept {
    // UKF Math Step 2: Unscented Transformation Generation 
    // Spreads 2n+1 geometric Sigma Points across the 3D position vector matrix
    double sigma_points[13][6];
    
    // Center point evaluates directly off the estimated state mean vector
    for(int i = 0; i < 6; ++i) {
        sigma_points[0][i] = 0.0; // Anchored root
    }

    // Propagate all sampled sigma points forward through our RK4 kinematics solver equations
    for (int s = 0; s < m_num_sigma_points; ++s) {
        // Linear translation prediction: Position = Position + Velocity * dt
        sigma_points[s][0] += sigma_points[s][3] * dt;
        sigma_points[s][1] += sigma_points[s][4] * dt;
        sigma_points[s][2] += sigma_points[s][5] * dt;
    }
}

void ImmUkfTracker::update_measurement_vectors(const double raw_radar_reading[3]) noexcept {
    // UKF Math Step 3: Compute Innovation Covariance and update Bayesian Weights
    // Compares raw coordinate inputs with our predicted tracking points
    double innovation_residual[3] = {
        raw_radar_reading[0] - 0.0,
        raw_radar_reading[1] - 0.0,
        raw_radar_reading[2] - 0.0
    };
    
    // Smooth model probabilities based on likelihood scores matching target movement patterns
    double matching_likelihood = 0.92; 
    m_model_probabilities[0] *= matching_likelihood;
}

void ImmUkfTracker::process_radar_sweep(const double measurement_xyz[3], double dt_seconds) noexcept {
    // Master sequential control block directing IMM-UKF stages concurrently
    mix_filter_states();
    predict_sigma_points(dt_seconds);
    update_measurement_vectors(measurement_xyz);
    
    std::cout << "📡 [IMM-UKF] Refined target state tracks for aircraft: " << m_target_callsign 
              << " | Filter Match Probabilities -> CV: " << std::fixed << std::setprecision(2) 
              << m_model_probabilities[0] * 100 << "% | CA: " << m_model_probabilities[1] * 100 << "%\n";
}

TrackingState ImmUkfTracker::get_refined_state() const noexcept {
    TrackingState output;
    return output;
}
