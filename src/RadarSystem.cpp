#include "RadarSystem.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <vector>
#include <random>

#if defined(_WIN32)
    #define RESIZE_AVAILABLE 0
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
    #define RESIZE_AVAILABLE 1
#endif

// Static environmental definitions for SkyWatch-20
struct StormCell {
    double x, y;
    double radius;
};

const std::vector<StormCell> g_storms = {
    {8.0, 6.0, 4.0},   // Severe storm cell in Upper Right quadrant
    {-10.0, -8.0, 5.0} // Massive turbulence zone in Lower Left quadrant
};

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
    
    // Seed standard pseudo-random number generator for turbulence physics jitter
    static std::mt19937 rng(1337);
    std::uniform_real_distribution<double> dist(-0.4, 0.4);

    for (auto& ac : m_tracked_aircraft) {
        ac.update_position(dt_seconds);

        // FEATURE 2: Turbulence Zone Interaction Check
        Vector3D pos = ac.get_position();
        for (const auto& storm : g_storms) {
            double dx = pos.x - storm.x;
            double dy = pos.y - storm.y;
            if (std::sqrt(dx*dx + dy*dy) <= storm.radius) {
                // Apply crosswind jitter directly to positioning matrix
                ac.update_position(1.0); // Micro-step jitter delta
                Vector3D current = ac.get_position();
                // Safe const_cast bypass to simulate environmental wind pushing the hardware frame
                Vector3D* mutable_pos = const_cast<Vector3D*>(&ac.get_position());
                mutable_pos->x += dist(rng);
                mutable_pos->y += dist(rng);
            }
        }
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

    int screen_height = 24;
    int screen_width = 48; 

#if RESIZE_AVAILABLE
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        screen_height = std::max(12, w.ws_row - 7);
        screen_width = std::max(12, (w.ws_col - 6) / 2);
    }
#endif

    // Primary character display plane and secondary meta layer for colors
    std::vector<std::vector<char>> grid(screen_height, std::vector<char>(screen_width, ' '));
    std::vector<std::vector<int>> color_layer(screen_height, std::vector<int>(screen_width, 0));

    int center_y = screen_height / 2;
    int center_x = screen_width / 2;

    // Static Tracking Sweeper Angular Velocity Step Line
    static double sweep_angle = 0.0;
    sweep_angle += 0.15; 
    if (sweep_angle >= 2.0 * M_PI) sweep_angle -= 2.0 * M_PI;

    // Map drawing transformations to convert workspace Nautical Miles down to display boxes
    auto to_screen = [&](double nm_x, double nm_y) -> std::pair<int, int> {
        int sx = static_cast<int>(std::round(nm_x / 1.5)) + center_x;
        int sy = static_cast<int>(std::round(nm_y / 1.5)) + center_y;
        return {sx, sy};
    };

    // FEATURE 3: Geopolitical Airspace Boundary Markers (Terminal Corridor Lines)
    // Draw outer airspace limits ring border bounds using basic math boxes
    for (int x = 2; x < screen_width - 2; ++x) {
        grid[2][x] = '-';             color_layer[2][x] = 33; // Yellow boundary border
        grid[screen_height - 3][x] = '-'; color_layer[screen_height - 3][x] = 33;
    }
    for (int y = 2; y < screen_height - 2; ++y) {
        grid[y][2] = '|';            color_layer[y][2] = 33;
        grid[y][screen_width - 3] = '|'; color_layer[y][screen_width - 3] = 33;
    }

    // FEATURE 2: Render Storm Cells on Visual Canvas
    for (const auto& storm : g_storms) {
        for (double r_x = -storm.radius; r_x <= storm.radius; r_x += 0.5) {
            for (double r_y = -storm.radius; r_y <= storm.radius; r_y += 0.5) {
                if (r_x*r_x + r_y*r_y <= storm.radius*storm.radius) {
                    auto [sx, sy] = to_screen(storm.x + r_x, storm.y + r_y);
                    if (sx >= 0 && sx < screen_width && sy >= 0 && sy < screen_height) {
                        if (grid[sy][sx] == ' ') {
                            grid[sy][sx] = ' '; // Keeps field blank but marks background color context
                            color_layer[sy][sx] = 31; // Dark Red Alert Background
                        }
                    }
                }
            }
        }
    }

    // Anchor center tracking crosshair station
    grid[center_y][center_x] = 'o';
    color_layer[center_y][center_x] = 32;

    // Track active aircraft plots onto our tactical projection board
    for (const auto& ac : m_tracked_aircraft) {
        Vector3D pos = ac.get_position();
        auto [sx, sy] = to_screen(pos.x, pos.y);

        if (sx >= 0 && sx < screen_width && sy >= 0 && sy < screen_height) {
            // FEATURE 1: Sonic Sweep Angle Line & Phosphorus Decay Calculations
            double target_angle = std::atan2(pos.y, pos.x);
            if (target_angle < 0) target_angle += 2.0 * M_PI;

            double angle_diff = std::abs(sweep_angle - target_angle);
            if (angle_diff > M_PI) angle_diff = (2.0 * M_PI) - angle_diff;

            // Determine visibility state based on proximity to the sweeping hardware beam
            if (angle_diff < 0.3) {
                grid[sy][sx] = '+';
                color_layer[sy][sx] = 96; // Bold Glowing Cyan (Fresh Hit)
            } else if (angle_diff < 1.2) {
                grid[sy][sx] = '+';
                color_layer[sy][sx] = 32; // Standard Green (Active Sweep Trace)
            } else {
                grid[sy][sx] = '.';
                color_layer[sy][sx] = 2;  // Dim Low-Visibility Dark Green (Phosphorus Decay)
            }

            // High Threat Override: If aircraft compromises safety perimeters, force blinking highlight red
            if (sx <= 2 || sx >= screen_width - 3 || sy <= 2 || sy >= screen_height - 3) {
                grid[sy][sx] = 'X';
                color_layer[sy][sx] = 51; // Flashing Red Threat Marker
            }
        }
    }

    // Stream canvas arrays to hardware pipeline with precise ANSI text styling loops
    std::cout << "\033[H\033[J"; 
    std::cout << "\033[1;32m================= SKYWATCH-20 TACTICAL RADAR =================\033[0m\n";
    for (int y = screen_height - 1; y >= 0; --y) {
        std::cout << " ";
        for (int x = 0; x < screen_width; ++x) {
            int code = color_layer[y][x];
            if (code == 96) {
                std::cout << "\033[1;96m" << grid[y][x] << "\033[0m "; // Active Cyan
            } else if (code == 32) {
                std::cout << "\033[1;32m" << grid[y][x] << "\033[0m "; // Clear Green 
            } else if (code == 2) {
                std::cout << "\033[2;32m" << grid[y][x] << "\033[0m "; // Decayed Trace
            } else if (code == 33) {
                std::cout << "\033[1;33m" << grid[y][x] << "\033[0m "; // Border Line
            } else if (code == 31) {
                std::cout << "\033[31m" << "░" << "\033[0m ";          // Weather Cloud fill
            } else if (code == 51) {
                std::cout << "\033[1;5;31m" << "X" << "\033[0m ";      // Perimeter Warning
            } else {
                std::cout << "  ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\033[1;32m==============================================================\033[0m\n";
}
