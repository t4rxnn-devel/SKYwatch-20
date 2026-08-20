#include <iostream>
#include <vector>
#include <string>
#include <cmath>

// SkyWatch-20 Unscented Kalman Filter Model Mixer Pipeline
// Simulated multi-model state vector array logic

class ImmUkfTracker {
private:
    std::string m_callsign;
    double m_model_probabilities[2]; // [Constant Velocity, Constant Acceleration]

public:
    ImmUkfTracker(std::string callsign) {
        m_callsign = callsign;
        m_model_probabilities[0] = 0.85; // Initial bias to smooth flight
        m_model_probabilities[1] = 0.15;
    }

    void update_filter_matrices(double raw_x, double raw_y, double dt) {
        // Interacting Multiple Model (IMM) Likelihood Update Simulation
        double innovation_error = std::sqrt(raw_x * raw_x + raw_y * raw_y);
        
        if (innovation_error > 15.0) {
            // Target is maneuvering heavily - shift tracking weight to acceleration model
            m_model_probabilities[0] = 0.30;
            m_model_probabilities[1] = 0.70;
        } else {
            m_model_probabilities[0] = 0.90;
            m_model_probabilities[1] = 0.10;
        }
        
        std::cout << "📡 [C++ IMM-UKF]: " << m_callsign 
                  << " Track Converged | Model Match -> CV: " 
                  << (m_model_probabilities[0] * 100.0) << "% | CA: " 
                  << (m_model_probabilities[1] * 100.0) << "%\n";
    }
};
