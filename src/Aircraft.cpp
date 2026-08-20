#include "Aircraft.hpp"
#include <utility>
#include <cmath>

Aircraft::Aircraft(std::string id, double speed, Vector3D pos, std::deque<Vector3D> waypoints)
    : m_id(std::move(id)), m_speed(speed), m_position(pos), m_waypoints(std::move(waypoints)) {
    recalculate_velocity();
}

void Aircraft::recalculate_velocity() noexcept {
    if (m_waypoints.empty()) {
        m_velocity = {0.0, 0.0, 0.0};
        return;
    }

    Vector3D target = m_waypoints.front();
    Vector3D direction = target - m_position;
    double dist = std::sqrt(direction.dot(direction));

    if (dist > 0.1) {
        double speed_nm_per_sec = (m_speed / 3600.0);
        m_velocity = (direction * (1.0 / dist)) * speed_nm_per_sec;
    } else {
        m_velocity = {0.0, 0.0, 0.0};
    }
}

void Aircraft::apply_tcas_evasion() noexcept {
    if (!m_has_tcas_climb) {
        m_position.z += 0.164; // +1000 feet in Nautical Miles
        m_has_tcas_climb = true;
        recalculate_velocity();
    }
}

void Aircraft::update_position(double dt_seconds) noexcept {
    if (m_waypoints.empty()) return;

    m_position = m_position + (m_velocity * dt_seconds);

    if (m_position.distance_to(m_waypoints.front()) < 1.0) {
        m_waypoints.pop_front();
        recalculate_velocity();
    }
}
