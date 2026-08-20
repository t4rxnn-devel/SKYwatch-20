#pragma once
#include <SFML/Graphics.hpp>
#include "Aircraft.hpp"
#include "PhysicsSolver.hpp"
#include <vector>

class RadarGuiLayout {
private:
    unsigned int m_width;
    unsigned int m_height;
    float m_scale_factor;

    void draw_tactical_grid(sf::RenderWindow& window) const;
    void draw_weather_cells(sf::RenderWindow& window, const std::vector<WeatherZone>& storms) const;
    void draw_sweeper_beam(sf::RenderWindow& window, float current_angle) const;
    void draw_fleet_targets(sf::RenderWindow& window, const std::vector<Aircraft>& fleet) const;

public:
    RadarGuiLayout(unsigned int width, unsigned int height) noexcept;
    
    // Executes the master render frame sequence
    void render_tactical_frame(sf::RenderWindow& window, 
                               const std::vector<Aircraft>& fleet, 
                               const std::vector<WeatherZone>& storms, 
                               float sweep_angle) const;
};
