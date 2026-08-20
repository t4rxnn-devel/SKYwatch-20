#pragma once
#include "Aircraft.hpp"
#include <vector>
#include <mutex>
#include <unordered_map>

struct GridKey {
    int x, y, z;
    bool operator==(const GridKey& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

struct GridKeyHash {
    std::size_t operator()(const GridKey& k) const {
        return ((std::hash<int>()(k.x) ^ (std::hash<int>()(k.y) << 1)) >> 1) ^ (std::hash<int>()(k.z) << 1);
    }
};

class RadarSystem {
private:
    std::vector<Aircraft> m_tracked_aircraft;
    double m_collision_threshold;
    double m_lookahead_time;
    double m_grid_cell_size; 
    mutable std::mutex m_radar_mutex; // Protects data across background threads

    [[nodiscard]] GridKey get_grid_key(const Vector3D& pos) const noexcept;

public:
    RadarSystem(double safety_radius_nm, double lookahead_seconds) noexcept;

    void track_aircraft(const Aircraft& ac);
    void simulate_time_step(double dt_seconds) noexcept;
    void process_spatial_tcas_alerts() noexcept;
    void render_ansi_visualizer() const noexcept;
};
