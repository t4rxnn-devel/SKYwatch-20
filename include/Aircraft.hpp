#pragma once
#include "Vector3D.hpp"
#include <string>
#include <deque>

class Aircraft {
private:
    std::string m_id;
    double m_speed; 
    Vector3D m_position; 
    Vector3D m_velocity; 
    std::deque<Vector3D> m_waypoints;
    bool m_has_tcas_climb = false;

public:
    Aircraft(std::string id, double speed, Vector3D pos, std::deque<Vector3D> waypoints);

    [[nodiscard]] std::string get_id() const noexcept { return m_id; }
    [[nodiscard]] double get_speed() const noexcept { return m_speed; }
    [[nodiscard]] Vector3D get_position() const noexcept { return m_position; }
    [[nodiscard]] Vector3D get_velocity() const noexcept { return m_velocity; }
    
    void apply_tcas_evasion() noexcept;
    void update_position(double dt_seconds) noexcept;
    void recalculate_velocity() noexcept;
};
