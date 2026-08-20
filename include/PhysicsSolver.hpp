#pragma once
#include "Aircraft.hpp"
#include <vector>

struct WeatherZone {
    double x, y, radius, intensity;
};

class PhysicsSolver {
public:
    // Applies 3D kinematic vector calculations, heading interpolations, and environmental friction
    static void compute_flight_dynamics(std::vector<Aircraft>& fleet, 
                                        const std::vector<WeatherZone>& storms, 
                                        double dt_seconds) noexcept;
};
