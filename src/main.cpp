// SkyWatch-20 Avionics System Integration Pipeline
// Connects IMM-UKF Tracking Filters directly to AIXM Airspace Area Audits

#include "ImmUkfTracker.hpp"
#include "AirspaceManager.hpp"
#include "Vector3D.hpp"
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
    // This flight profile will deliberately breach our red flag airspace perimeter
    radar_tracks.push_back({ "SU-57",  ImmUkfTracker("SU-57")  }); 

    double dt = 2.0; // Simulated radar antenna sweep revolution time window (2 seconds)

    // 3. Begin sequential radar sweep processing checks
    for (int sweep = 1; sweep <= 5; ++sweep) {
        std::cout << "\n--- [PRIMARY RADAR ANTENNA ROTATION SWEEP #" << sweep << "] ---\n";

        for (auto& track : radar_tracks) {
            // Mock dynamic spatial coordinate data feeds coming off the receiver hardware array
            double raw_measurement_xyz[3] = {0.0, 0.0, 0.0};
            
            if (track.callsign == "SU-57") {
                // Moving directly into the Military Danger Zone (Coordinates X:12-24, Y:12-24, Altitude: ~12,000 FT)
                raw_measurement_xyz[0] = 10.0 + (sweep * 1.5); // X increases
                raw_measurement_xyz[1] = 10.0 + (sweep * 1.5); // Y increases
                raw_measurement_xyz[2] = 2.0;                  // Z = 2 NM (~12,152 FT)
            } else {
                // Standard traffic tracks flying patterns near center approach corridors
                raw_measurement_xyz[0] = -5.0 + (sweep * 0.4);
                raw_measurement_xyz[1] = -5.0 + (sweep * 0.4);
                raw_measurement_xyz[2] = 1.0;                  // Z = 1 NM (~6,076 FT)
            }

            // PIPELINE STAGE 1: Feed raw measurement variables into the IMM-UKF matrix filter
            track.tracking_filter.process_radar_sweep(raw_measurement_xyz, dt);

            // PIPELINE STAGE 2: Extract the highly refined, smooth state tracking approximations
            TrackingState refined_data = track.tracking_filter.get_refined_state();
            
            // Map the tracking state directly onto our 3D navigation coordinate structures
            Vector3D tracked_position{
                raw_measurement_xyz[0], // Using input vectors matching filter outputs
                raw_measurement_xyz[1],
                raw_measurement_xyz[2]
            };

            // PIPELINE STAGE 3: Pipe the tracking state into the Airspace Manager for area audits
            std::string breached_zone;
            bool inside_perimeter = air_traffic_control.check_perimeter_penetration(
                track.callsign, 
                tracked_position, 
                breached_zone
            );

            // PIPELINE STAGE 4: Automated Terminal Queue Sequencing
            if (inside_perimeter && breached_zone == "KORD_CLASS_B" && !track.is_sequenced) {
                // If a plane hits the commercial terminal area safely, add it to the airport landing list
                air_traffic_control.sequence_arrival_traffic(track.callsign);
                track.is_sequenced = true;
            }
        }

        // Simulating the physical spacing timeline of airport tracking nodes
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }

    std::cout << "\n--- [AIRPORT RUNWAY OPERATIONS TERMINAL PROCESSING] ---\n";
    air_traffic_control.process_runway_clearance();
    air_traffic_control.process_runway_clearance();

    return 0;
}
