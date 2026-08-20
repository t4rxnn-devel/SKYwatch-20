#include "RadarSystem.hpp"
#include "PhysicsSolver.hpp"
#include "RadarGuiLayout.hpp"
#include <SFML/Graphics.hpp>
#include <vector>

int main() {
    // Generate clean environment zones
    std::vector<WeatherZone> active_storms = {
        {10.0, 8.0, 5.0, 1.2},   
        {-12.0, -10.0, 6.0, 1.8} 
    };

    // Instantiate base tracking datasets
    std::vector<Aircraft> fleet;
    fleet.push_back(Aircraft("UAL104", 480.0, {-15.0, -15.0, 5.0}, {{-5.0, -5.0, 5.0}, {15.0, 15.0, 5.0}}));
    fleet.push_back(Aircraft("AAL892", 460.0, {15.0, 15.0, 4.9}, {{5.0, 5.0, 4.9}, {-15.0, -15.0, 4.9}}));
    fleet.push_back(Aircraft("DAL440", 500.0, {-18.0, 10.0, 6.0}, {{0.0, 0.0, 6.0}, {18.0, -10.0, 6.0}}));

    // Setup visual dimensions 
    sf::RenderWindow window(sf::VideoMode(800, 800), "SkyWatch-20 Tactical Interface System", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    RadarGuiLayout ui_manager(800, 800);
    float sweep_angle = 0.0f;

    // GUI Master Window Lifecycle Execution loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Run isolated calculations away from standard graphic steps
        PhysicsSolver::compute_flight_dynamics(fleet, active_storms, 0.016f); // 60 FPS tick rates

        // Increment scanline parameters smoothly
        sweep_angle += 0.02f;
        if (sweep_angle >= 2.0f * 3.14159f) sweep_angle -= 2.0f * 3.14159f;

        // Render graphical matrix
        ui_manager.render_tactical_frame(window, fleet, active_storms, sweep_angle);
    }

    return 0;
}
