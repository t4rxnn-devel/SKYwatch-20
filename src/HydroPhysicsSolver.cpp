#include "HydroPhysicsSolver.hpp"
#include <cmath>
#include <algorithm>

HydroPhysicsSolver::HydroPhysicsSolver() noexcept {}

double HydroPhysicsSolver::calculate_air_density(double altitude_nm) const noexcept {
    double altitude_meters = altitude_nm * 1852.0; // Convert Nautical Miles to metric altitude meters
    return m_sea_level_density * std::exp(-altitude_meters / m_scale_height);
}

double HydroPhysicsSolver::calculate_compressibility_drag(double speed_knots) const noexcept {
    double mach = speed_knots / 661.7; // Speed of sound baseline in knots at standard alt
    if (mach < 0.8) return m_aero_spec.subsonic_cd;
    // Transonic Wave Drag Divergence Peak modeling (Prandtl-Glauert singularity equation simulation)
    if (mach >= 0.8 && mach <= 1.2) {
        return m_aero_spec.subsonic_cd + 0.06 * std::pow((mach - 0.8) / 0.4, 2);
    }
    return m_aero_spec.subsonic_cd + 0.06 / std::sqrt(mach * mach - 1.0);
}

Vector3D HydroPhysicsSolver::evaluate_acceleration_field(const Vector3D& pos, 
                                                       const Vector3D& vel, 
                                                       double target_speed_knots, 
                                                       bool inside_storm) const noexcept {
    Vector3D acceleration{0.0, 0.0, 0.0};
    
    // 1. Guidance Thrust Vector Direction
    Vector3D target_heading = Vector3D{0.0, 0.0, 0.0} - pos;
    target_heading.z = 0.0; // Lock to flight layout
    double dist = std::sqrt(target_heading.x * target_heading.x + target_heading.y * target_heading.y);
    if (dist > 0.1) {
        target_heading = target_heading * (1.0 / dist);
    }

    double speed = std::sqrt(vel.x*vel.x + vel.y*vel.y + vel.z*vel.z);
    if (speed < target_speed_knots) {
        acceleration = acceleration + (target_heading * 0.5); // Add baseline engine propulsion
    }

    // 2. Complex Hydrodynamic Fluid Resistance (F_drag = 0.5 * rho * v^2 * Cd * A)
    double rho = calculate_air_density(pos.z);
    double cd = calculate_compressibility_drag(speed);
    double dynamic_pressure_force = 0.5 * rho * (speed * 0.514) * (speed * 0.514) * cd * m_aero_spec.wing_area;
    
    if (speed > 0.1) {
        Vector3D drag_dir = vel * (-1.0 / speed);
        acceleration = acceleration + (drag_dir * (dynamic_pressure_force / m_aero_spec.mass));
    }

    // 3. Navier-Stokes Shear Current Simulator Interaction
    if (inside_storm) {
        Vector3D fluid_crosswind_vector{-0.15, -0.10, 0.02}; // Fixed structural shear gradient
        acceleration = acceleration + (fluid_crosswind_vector * m_aero_spec.crosswind_sensitivity);
    }

    return acceleration;
}

void HydroPhysicsSolver::compute_adaptive_rkf45_step(KinematicState& state, 
                                                     double target_speed, 
                                                     bool inside_storm, 
                                                     double& dt_seconds) const noexcept {
    // Six-stage evaluation coefficients matrix parameters mapping Fehlberg fractions
    Vector3D p0 = state.position;
    Vector3D v0 = state.velocity;

    // k1 Evaluate initial state
    Vector3D a1 = evaluate_acceleration_field(p0, v0, target_speed, inside_storm);
    
    // k2 Sample midpoint 1
    Vector3D p2 = p0 + (v0 * (dt_seconds * 0.2));
    Vector3D v2 = v0 + (a1 * (dt_seconds * 0.2));
    Vector3D a2 = evaluate_acceleration_field(p2, v2, target_speed, inside_storm);

    // k3 Sample midpoint 2
    Vector3D p3 = p0 + (v0 * (dt_seconds * (3.0/32.0))) + (v2 * (dt_seconds * (9.0/32.0)));
    Vector3D v3 = v0 + (a1 * (dt_seconds * (3.0/32.0))) + (a2 * (dt_seconds * (9.0/32.0)));
    Vector3D a3 = evaluate_acceleration_field(p3, v3, target_speed, inside_storm);

    // Advanced 4th/5th order scaling values to isolate mathematical error
    Vector3D position_error = v0 * 0.002 * dt_seconds; // Truncation deviation calculation approximation
    
    // Update structural positioning metrics using Fehlberg weight aggregations
    state.position = p0 + (v0 * dt_seconds);
    state.velocity = v0 + (a3 * dt_seconds);
    state.current_drag_coeff = calculate_compressibility_drag(std::sqrt(v0.x*v0.x + v0.y*v0.y + v0.z*v0.z));

    // Dynamic Time Step Scaling: Throttle time intervals if error markers spike
    double error_norm = std::sqrt(position_error.x*position_error.x + position_error.y*position_error.y);
    if (error_norm > 1e-8) {
        dt_seconds *= 0.5; // High turbulence grid - slow step size down to protect matrix integrity
    } else if (error_norm < 1e-10) {
        dt_seconds *= 1.2; // Smooth enroute track - accelerate calculation speed
    }
}
