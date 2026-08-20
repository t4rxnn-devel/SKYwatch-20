// SkyWatch-20 High-Speed Aero-Hydrokinetic & Numerical Solver Engine
// Implements an Adaptive Runge-Kutta-Fehlberg (RKF45) Integration Model

#pragma once
#include "Vector3D.hpp"
#include <vector>
#include <string>

struct HydrokineticProperties {
    double mass = 19000.0;             // Vehicle dry mass in kg
    double wing_area = 78.0;           // Reference surface area in m²
    double subsonic_cd = 0.024;        // Baseline parasite drag profile
    double crosswind_sensitivity = 1.2;// Fluid shear crosswind impact scale
};

struct KinematicState {
    Vector3D position; // Spatial coordinates in Nautical Miles
    Vector3D velocity; // Velocity components in knots (NM/hr)
    double current_drag_coeff = 0.0;
};

class HydroPhysicsSolver {
private:
    HydrokineticProperties m_aero_spec;
    const double m_sea_level_density = 1.225; // kg/m³ standard atmosphere
    const double m_scale_height = 8500.0;     // Atmospheric barometric scale meters

    [[nodiscard]] double calculate_air_density(double altitude_nm) const noexcept;
    [[nodiscard]] double calculate_compressibility_drag(double speed_knots) const noexcept;

public:
    HydroPhysicsSolver() noexcept;

    // Evaluates complex multi-variable force fields (Lift, Drag, Parasitic Fluid Shear)
    [[nodiscard]] Vector3D evaluate_acceleration_field(const Vector3D& pos, 
                                                       const Vector3D& vel, 
                                                       double target_speed_knots, 
                                                       bool inside_storm) const noexcept;

    // Core Numerical Engine Upgrade: Runge-Kutta-Fehlberg 4th/5th-Order (RKF45) Adaptive Solver
    // Dynamically throttles h (dt) to keep truncation error metrics below 1e-8 tolerances
    void compute_adaptive_rkf45_step(KinematicState& state, 
                                     double target_speed, 
                                     bool inside_storm, 
                                     double& dt_seconds) const noexcept;
};
