#include "RadarSystem.hpp"
#include <thread>
#include <chrono>

int main() {
    RadarSystem control_tower(5.0, 60.0);

    // Initial setup pushing multiple variants across intersecting flight tracks
    control_tower.track_aircraft(Aircraft("UAL104", 480.0, {-15.0, -15.0, 5.0}, {{-5.0, -5.0, 5.0}, {15.0, 15.0, 5.0}}));
    control_tower.track_aircraft(Aircraft("AAL892", 460.0, {15.0, 15.0, 4.9}, {{5.0, 5.0, 4.9}, {-15.0, -15.0, 4.9}}));
    control_tower.track_aircraft(Aircraft("DAL440", 500.0, {-18.0, 10.0, 6.0}, {{0.0, 0.0, 6.0}, {18.0, -10.0, 6.0}}));

    for (int step = 0; step < 50; ++step) {
        control_tower.simulate_time_step(2.0); 
        control_tower.process_spatial_tcas_alerts();
        control_tower.render_ansi_visualizer();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    return 0;
}
