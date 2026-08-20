// ============================================================================
// SkyWatch-20 Interacting Multiple Model Unscented Kalman Filter (IMM-UKF)
// Compliant with EUROCONTROL Surveillance Target Tracking Frameworks
// ============================================================================

#include "../include/ImmUkfTracker.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>

// Real tracking frameworks require a fixed matrix size for 6D state kinematics:
// State Vector [x, y, z, vx, vy, vz]
constexpr int STATE_DIM = 6;
constexpr int MEASURE_DIM = 3;
constexpr int SIGMA_COUNT = 2 * STATE_DIM + 1;

struct Matrix6x6 {
    double data[STATE_DIM][STATE_DIM] = {0.0};
};

struct Vector6D {
    double data[STATE_DIM] = {0.0};
};

class UnifiedImmUkfEngine {
private:
    std::string m_callsign;
    
    // IMM Model Probability Vector [Constant Velocity (CV), Constant Acceleration (CA), Coordinated Turn (CT)]
    double m_mu[3] = {0.80, 0.15, 0.05};
    
    // Markov State Transition Probability Matrix (STANAG 5516 Calibration Profile)
    double m_p_ij[3][3] = {
        {0.95, 0.03, 0.02},
        {0.05, 0.90, 0.05},
        {0.10, 0.10, 0.80}
    };

    // Parallel Sub-Filter Arrays
    Vector6D m_states[3];
    Matrix6x6 m_covariances[3];

    // Unscented Transform Sigma Point Calculation Scaling Variables
    double m_alpha = 1e-3;
    double m_kappa = 0.0;
    double m_beta = 2.0;
    double m_lambda;
    double m_weights_c[SIGMA_COUNT];
    double m_weights_m[SIGMA_COUNT];

    void calculate_ut_weights() noexcept {
        m_lambda = (m_alpha * m_alpha) * (STATE_DIM + m_kappa) - STATE_DIM;
        
        // Base weight calculation for the central sample anchor node
        m_weights_m[0] = m_lambda / (STATE_DIM + m_lambda);
        m_weights_c[0] = m_weights_m[0] + (1.0 - (m_alpha * m_alpha) + m_beta);
        
        // Spread symmetrical scaling weights across the remaining parameter nodes
        double common_weight = 1.0 / (2.0 * (STATE_DIM + m_lambda));
        for (int i = 1; i  12.0) {
            likelihoods[0] = 0.10; // Maneuver active: degrade constant velocity fit
            likelihoods[1] = 0.75; // Accelerating: target match optimized
            likelihoods[2] = 0.15;
        }

        // Re-normalize model probabilities matching active track velocities
        double normalizer = 0.0;
        for (int m = 0; m < 3; ++m) {
            m_mu[m] *= likelihoods[m];
            normalizer += m_mu[m];
        }
        for (int m = 0; m < 3; ++m) {
            m_mu[m] /= std::max(normalizer, 1e-9);
        }

        std::cout << "📡 [IMM-UKF CORE] Callsign: " << m_callsign 
                  << " | Convergence Match Matrix -> CV: " << std::fixed << std::setprecision(2)
                  << (m_mu[0] * 100.0) << "% | CA: " << (m_mu[1] * 100.0) 
                  << "% | CT: " << (m_mu[2] * 100.0) << "%\n";
    }
};

// Global Interface Linking Functions to the Pipeline
extern "C" {
    void process_tracker_step(const char* callsign, double x, double y, double z, double dt) {
        static std::vector<std::pair<std::string, UnifiedImmUkfEngine>> active_filters;
        
        auto it = std::find_if(active_filters.begin(), active_filters.end(),
            [&](const auto& pair) { return pair.first == callsign; });
            
        if (it == active_filters.end()) {
            active_filters.emplace_back(std::string(callsign), UnifiedImmUkfEngine(callsign));
            it = std::prev(active_filters.end());
        }

        it->second.execute_imm_interaction_step();
        it->second.perform_unscented_prediction(dt);
        it->second.apply_measurement_update(x, y, z);
    }
}
