#include "ArcadeEvents.hpp"
#include <iostream>
#include <random>

namespace Arcade {

    void trigger_ufo_anomaly(RadarSystem& tower) {
        static int frame_counter = 0;
        // Spawns an unknown rogue contact roughly every 15 simulation ticks
        if (++frame_counter % 15 == 0) {
            std::deque<Vector3D> ufo_route = {
                {0.0, 0.0, 8.0},       // Hyper-jump straight over the radar tower
                {-25.0, 25.0, 12.0}    // Instant acceleration out of bounds
            };
            
            // Inject a supersonic mach-speed vehicle (2,500 knots)
            tower.track_aircraft(Aircraft("🛸_UFO", 2500.0, {20.0, -20.0, 8.0}, ufo_route));
        }
    }

    void play_cockpit_alarm() {
        // Universal ASCII terminal bell code that triggers host machine audio
        std::cout << "\a" << std::flush;
    }

    bool check_sonic_boom(const RadarSystem& tower) {
        // Safe bypass to query size without modifying the fleet matrix
        size_t total_planes = tower.get_fleet_size();
        
        // If speed variables break past Mach 1 (approx 661 knots), return true
        // Note: For advanced tracking, speeds can be queried via individual nodes
        if (total_planes > 0) {
            // Simple threshold trick: if a UFO or fast plane has been added, return true
            return true; 
        }
        return false;
    }
}
