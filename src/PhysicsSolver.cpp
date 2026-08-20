#include "PhysicsSolver.hpp"
#include <cmath>

void PhysicsSolver::compute_flight_dynamics(std::vector<Aircraft>& fleet, 
                                            const std::vector<WeatherZone>& storms, 
                                            double dt_seconds) noexcept {
    for (auto& ac : fleet) {
        // Core kinematic position updates via current structural velocity vectors
        ac.update_position(dt_seconds);

        // Environmental crosswind injection if passing through friction zones
        Vector3D pos = ac.get_position();
        for (const auto& storm : storms) {
            double dx = pos.x - storm.x;
            double dy = pos.y - storm.y;
            double dist = std::sqrt(dx * dx + dy * dy);
            
            if (dist <= storm.radius) {
                // Apply subtle lateral drift vectors directly scaled by storm metrics
                double micro_drift = (1.0 - (dist / storm.radius)) * storm.intensity * dt_seconds;
                Vector3D* mutable_pos = const_cast<Vector3D*>(&ac.get_position());
                mutable_pos->x += micro_drift * 0.1;
                mutable_pos->y -= micro_drift * 0.05; // Asymmetric shear
            }
        }
    }
}
