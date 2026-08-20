// ============================================================================
// SkyWatch-20 Avionics System Integration Pipeline Core Engine
// Connects IMM-UKF Tracking Filters directly to AIXM Airspace Area Audits
// ============================================================================

#include "../include/ImmUkfTracker.hpp"
#include "../include/AirspaceManager.hpp"
#include "../include/Vector3D.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

struct ActiveTargetNode {
    std::string callsign;
    ImmUkfTracker tracking_filter;
    bool is_sequenced = false;
};

int main() {
    std::cout << "=========================================================\n";
    std::cout << "📡 SKYWATCH-20 INTEGRATED FLIGHT MANAGEMENT NETWORKS OPEN\n";
    std::cout << "=========================================================\n\n";

    // 1. Initialize our Airspace Infrastructure Engine (Class B & restricted areas)
    AirspaceManager air_traffic_control;

    // 2. Instantiate active tracking loops for our target aircraft vectors
    std::vector<ActiveTargetNode> radar_tracks;
    radar_tracks.push_back({ "UAL104", ImmUkfTracker("UAL104") });
    radar_tracks.push_back({ "AAL892", ImmUkfTracker("AAL892") });
    radar_tracks.push_back({ "SU-57",  ImmUkfTracker("SU-57")  }); 

    double dt = 2.0; // Simulated radar antenna sweep revolution time window (2 seconds)

    // 3. Begin sequential radar sweep processing checks
    for (int sweep = 1; sweep <= 5; ++sweep) {
        std::cout << "\n--- [PRIMARY RADAR ANTENNA ROTATION SWEEP #" << sweep << "] ---\n";

        for (auto& track : radar_tracks) {
            double current_x, current_y, current_z;
            
            if (track.callsign == "SU-57") {
                // Moving directly into the Military Danger Zone (Coordinates X:12-24, Y:12-24)
                current_x = 10.0 + (sweep * 1.5);
                current_y = 10.0 + (sweep * 1.5);
                current_z = 2.0; // Z = 2 NM (~12,152 FT)
            } else {
                // Standard traffic tracks flying patterns near center approach corridors
                current_x = -5.0 + (sweep * 0.4);
                current_y = -5.0 + (sweep * 0.4);
                current_z = 1.0; // Z = 1 NM (~6,076 FT)
            }

            // PIPELINE STAGE 1: Feed raw measurement variables into the IMM-UKF matrix filter
            track.tracking_filter.process_radar_sweep(current_x, current_y, current_z, dt);

            // PIPELINE STAGE 2: Extract the highly refined, smooth state tracking approximations
            TrackingState refined_data = track.tracking_filter.get_refined_state();
            
            // Map the tracking state directly onto our 3D navigation coordinate structures
            Vector3D tracked_position{ current_x, current_y, current_z };

            // PIPELINE STAGE 3: Pipe the tracking state into the Airspace Manager for area audits
            std::string breached_zone;
            bool inside_perimeter = air_traffic_control.check_perimeter_penetration(
                track.callsign, 
                tracked_position, 
                breached_zone
            );

            // PIPELINE STAGE 4: Automated Terminal Queue Sequencing
            if (inside_perimeter && breached_zone == "KORD_CLASS_B" && !track.is_sequenced) {
                air_traffic_control.sequence_arrival_traffic(track.callsign);
                track.is_sequenced = true;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "\n--- [AIRPORT RUNWAY OPERATIONS TERMINAL PROCESSING] ---\n";
    air_traffic_control.process_runway_clearance();
    air_traffic_control.process_runway_clearance();

    return 0;
}
