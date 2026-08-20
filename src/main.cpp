#include "RadarSystem.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib> // Required for native system call operations

int main() {
    // 1. SILENT LAUNCH LAYER: Automatically kick off the parallel grid binary 
    // This executes your parallel algorithms natively without user input required.
    std::cout << "\033[1;33m[SYSTEM INIT] Invoking parallel GPU co-processor nodes...\033[0m\n";
    
    // Auto-compiles or runs the parallel component cleanly in the background
    #if defined(_WIN32)
        std::system("bend run scripts\\parallel_grid.bend > nul 2>&1");
    #else
        std::system("bend run scripts/parallel_grid.bend > /dev/null 2>&1 &");
    #endif

    // 2. CORE ENGINE INITIALIZATION
    RadarSystem control_tower(5.0, 60.0);

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
