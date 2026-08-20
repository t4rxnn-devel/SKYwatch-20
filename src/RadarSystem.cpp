#include "RadarSystem.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <vector>

// Platform detection for terminal sizing
#if defined(_WIN32)
    #define RESIZE_AVAILABLE 0
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
    #define RESIZE_AVAILABLE 1
#endif

RadarSystem::RadarSystem(double safety_radius_nm, double lookahead_seconds) noexcept
    : m_collision_threshold(safety_radius_nm), m_lookahead_time(lookahead_seconds), m_grid_cell_size(10.0) {}

GridKey RadarSystem::get_grid_key(const Vector3D& pos) const noexcept {
    return {
        static_cast<int>(std::floor(pos.x / m_grid_cell_size)),
        static_cast<int>(std::floor(pos.y / m_grid_cell_size)),
        static_cast<int>(std::floor(pos.z / m_grid_cell_size))
    };
}

void RadarSystem::track_aircraft(const Aircraft& ac) {
    std::lock_guard<std::mutex> lock(m_radar_mutex);
    m_tracked_aircraft.push_back(ac);
}

void RadarSystem::simulate_time_step(double dt_seconds) noexcept {
    std::lock_guard<std::mutex> lock(m_radar_mutex);
    for (auto& ac : m_tracked_aircraft) {
        ac.update_position(dt_seconds);
    }
}

void RadarSystem::process_spatial_tcas_alerts() noexcept {
    std::lock_guard<std::mutex> lock(m_radar_mutex);
    if (m_tracked_aircraft.size() < 2) return;

    std::unordered_map<GridKey, std::vector<size_t>, GridKeyHash> spatial_grid;
    for (size_t i = 0; i < m_tracked_aircraft.size(); ++i) {
        spatial_grid[get_grid_key(m_tracked_aircraft[i].get_position())].push_back(i);
    }

    for (const auto& [key, indices] : spatial_grid) {
        for (size_t i = 0; i < indices.size(); ++i) {
            for (size_t j = i + 1; j < indices.size(); ++j) {
                auto& ac1 = m_tracked_aircraft[indices[i]];
                auto& ac2 = m_tracked_aircraft[indices[j]];

                Vector3D dP = ac1.get_position() - ac2.get_position();
                Vector3D dV = ac1.get_velocity() - ac2.get_velocity();

                double v2 = dV.dot(dV);
                double time_to_cpa = (v2 > 0.0) ? -(dP.dot(dV)) / v2 : 0.0;
                if (time_to_cpa < 0.0) time_to_cpa = 0.0;

                Vector3D cpa_p1 = ac1.get_position() + (ac1.get_velocity() * time_to_cpa);
                Vector3D cpa_p2 = ac2.get_position() + (ac2.get_velocity() * time_to_cpa);
                
                if (cpa_p1.distance_to(cpa_p2) <= m_collision_threshold && time_to_cpa <= m_lookahead_time) {
                    std::cout << "\033[1;31m [🚨 TCAS RESOLUTION] Intercept course: " 
                              << ac1.get_id() << " & " << ac2.get_id() << "\033[0m\n";
                    
                    if (ac1.get_position().z <= ac2.get_position().z) {
                        ac1.apply_tcas_evasion();
                    } else {
                        ac2.apply_tcas_evasion();
                    }
                }
            }
        }
    }
}

void RadarSystem::render_ansi_visualizer() const noexcept {
    std::lock_guard<std::mutex> lock(m_radar_mutex);

    // Default sizing bounds if sizing engine is unsupported
    int screen_height = 20;
    int screen_width = 40; 

#if RESIZE_AVAILABLE
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        // Leave room for the text headers and borders
        screen_height = std::max(10, w.ws_row - 6);
        // Divide by 2 because terminal fonts are typically twice as tall as they are wide
        screen_width = std::max(10, (w.ws_col - 6) / 2);
    }
#endif

    // Dynamically allocate the canvas based on structural hardware queries
    std::vector<std::vector<char>> grid(screen_height, std::vector<char>(screen_width, ' '));

    int center_y = screen_height / 2;
    int center_x = screen_width / 2;
    grid[center_y][center_x] = 'o';

    for (const auto& ac : m_tracked_aircraft) {
        Vector3D pos = ac.get_position();
        
        // Dynamically scale mapping scaling vector to fit current canvas size
        int screen_x = static_cast<int>(pos.x / 2.0) + center_x;
        int screen_y = static_cast<int>(pos.y / 2.0) + center_y;

        if (screen_x >= 0 && screen_x < screen_width && screen_y >= 0 && screen_y < screen_height) {
            grid[screen_y][screen_x] = '+'; 
        }
    }

    std::cout << "\033[H\033[J"; 
    std::cout << "\033[1;32m=================== DYNAMIC RADAR SCOPE ===================\033[0m\n";
    for (int y = screen_height - 1; y >= 0; --y) {
        std::cout << " |";
        for (int x = 0; x < screen_width; ++x) {
            if (grid[y][x] == '+') {
                std::cout << "\033[1;36m+\033[0m "; 
            } else if (grid[y][x] == 'o') {
                std::cout << "\033[1;33mo\033[0m "; 
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "|\n";
    }
    std::cout << "\033[1;32m===========================================================\033[0m\n";
}
