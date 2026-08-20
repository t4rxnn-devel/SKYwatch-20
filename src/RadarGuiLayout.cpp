#include "RadarGuiLayout.hpp"
#include <cmath>

RadarGuiLayout::RadarGuiLayout(unsigned int width, unsigned int height) noexcept
    : m_width(width), m_height(height), m_scale_factor(15.0f) {}

void RadarGuiLayout::draw_tactical_grid(sf::RenderWindow& window) const {
    float cx = m_width / 2.0f;
    float cy = m_height / 2.0f;

    // Draw realistic concentric target rings (10 NM, 20 NM, 30 NM metrics)
    for (float radius = 50.f; radius <= 350.f; radius += 75.f) {
        sf::CircleShape ring(radius);
        ring.setPosition(cx - radius, cy - radius);
        ring.setFillColor(sf::Color::Transparent);
        ring.setOutlineThickness(1.2f);
        ring.setOutlineColor(sf::Color(0, 100, 0, 150)); // Dark military green
        window.draw(ring);
    }

    // Draw crosshair axes lines
    sf::Vertex x_axis[] = { sf::Vertex(sf::Vector2f(0.f, cy), sf::Color(0, 80, 0, 100)), sf::Vertex(sf::Vector2f(m_width, cy), sf::Color(0, 80, 0, 100)) };
    sf::Vertex y_axis[] = { sf::Vertex(sf::Vector2f(cx, 0.f), sf::Color(0, 80, 0, 100)), sf::Vertex(sf::Vector2f(cx, m_height), sf::Color(0, 80, 0, 100)) };
    window.draw(x_axis, 2, sf::Lines);
    window.draw(y_axis, 2, sf::Lines);
}

void RadarGuiLayout::draw_weather_cells(sf::RenderWindow& window, const std::vector<WeatherZone>& storms) const {
    float cx = m_width / 2.0f;
    float cy = m_height / 2.0f;

    for (const auto& storm : storms) {
        float scr_r = storm.radius * m_scale_factor;
        sf::CircleShape cell(scr_r);
        cell.setPosition(cx + (storm.x * m_scale_factor) - scr_r, cy - (storm.y * m_scale_factor) - scr_r);
        
        // Use semi-transparent orange/red overlays for modern Doppler signature modeling
        cell.setFillColor(sf::Color(200, 50, 0, 45)); 
        cell.setOutlineThickness(1.0f);
        cell.setOutlineColor(sf::Color(255, 60, 0, 60));
        window.draw(cell);
    }
}

void RadarGuiLayout::draw_sweeper_beam(sf::RenderWindow& window, float current_angle) const {
    float cx = m_width / 2.0f;
    float cy = m_height / 2.0f;
    float length = std::max(m_width, m_height) * 0.7f;

    float target_x = cx + length * std::cos(current_angle);
    float target_y = cy - length * std::sin(current_angle); // Flip screen Y axis

    sf::Vertex line[] = {
        sf::Vertex(sf::Vector2f(cx, cy), sf::Color(0, 255, 100, 200)),
        sf::Vertex(sf::Vector2f(target_x, target_y), sf::Color(0, 255, 100, 0)) // Elegant fade gradient
    };
    window.draw(line, 2, sf::Lines);
}

void RadarGuiLayout::draw_fleet_targets(sf::RenderWindow& window, const std::vector<Aircraft>& fleet) const {
    float cx = m_width / 2.0f;
    float cy = m_height / 2.0f;

    for (const auto& ac : fleet) {
        Vector3D pos = ac.get_position();
        float sx = cx + (pos.x * m_scale_factor);
        float sy = cy - (pos.y * m_scale_factor); // Scale map space cleanly to UI bounds

        // Realistic square blip indicator for standard transponder tracks
        sf::RectangleShape blip(sf::Vector2f(6.f, 6.f));
        blip.setPosition(sx - 3.f, sy - 3.f);
        
        // Critical alerting state changes icon configurations automatically
        if (pos.z > 6.0) {
            blip.setFillColor(sf::Color(0, 220, 255, 240)); // High altitude flight tracking (Cyan)
        } else {
            blip.setFillColor(sf::Color(0, 255, 120, 240)); // Normal flight tracking (Bright Green)
        }
        window.draw(blip);
    }
}

void RadarGuiLayout::render_tactical_frame(sf::RenderWindow& window, 
                                           const std::vector<Aircraft>& fleet, 
                                           const std::vector<WeatherZone>& storms, 
                                           float sweep_angle) const {
    window.clear(sf::Color(2, 15, 5)); // Authentic deep terminal backdrop shade
    
    draw_tactical_grid(window);
    draw_weather_cells(window, storms);
    draw_fleet_targets(window, fleet);
    draw_sweeper_beam(window, sweep_angle);
    
    window.display();
}
