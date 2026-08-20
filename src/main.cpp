#include "ImmUkfTracker.hpp"
#include "AirspaceManager.hpp"
#include "HydroPhysicsSolver.hpp"
#include "LogExporter.hpp"
#include <sstream>
#include <thread>
#include <chrono>

int main() {
    // Instantiate the exporter destination path inside local folder arrays
    LogExporter black_box("generated/skywatch_mission_log.txt");
    HydroPhysicsSolver physics_solver;
    AirspaceManager airspace_control;

    std::vector<std::string> callsigns = {"UAL104", "AAL892", "SU-57"};
    std::vector<ImmUkfTracker> trackers = {ImmUkfTracker("UAL104"), ImmUkfTracker("AAL892"), ImmUkfTracker("SU-57")};
    
    // Initializing tracking states structures parameters
    std::vector<KinematicState> fleet_states(3);
    fleet_states[0] = { {-19.0, -14.0, 1.0}, {420.0, 210.0, 0.0} }; // UAL104
    fleet_states[1] = { {17.0, 15.0, 1.2}, {-380.0, -180.0, 0.0} }; // AAL892
    fleet_states[2] = { {-15.0, 19.0, 2.0}, {550.0, 310.0, 0.0} };   // SU-57 High Alpha target

    double dt = 2.0; // Core radar rotation baseline timeline seconds

    black_box.capture_log_line("[SYSTEM INIT] Booting SkyWatch-20 Vector Processing Cores...");
    black_box.capture_log_line("[SYSTEM INIT] Loading Aero-Hydrokinetic Boundary Profile Matrices...");

    for (int sweep = 1; sweep <= 5; ++sweep) {
        std::stringstream ss_sweep;
        ss_sweep << "\n--- [PRIMARY RADAR ANTENNA ROTATION SWEEP #" << sweep << " | SIM TIME STEP: +" << (sweep - 1) * 2.0 << "s] ---";
        black_box.capture_log_line(ss_sweep.str());

        for (size_t i = 0; i < fleet_states.size(); ++i) {
            // Trigger fluid storm zone tracking checks if within hazard cells
            bool inside_storm = (callsigns[i] == "SU-57" && sweep >= 3);
            double adaptive_dt = dt;

            // ADVANCED STAGE 1: Execute the 4th-Order Adaptive Fehlberg Physics Solver
            physics_solver.compute_adaptive_rkf45_step(fleet_states[i], 550.0, inside_storm, adaptive_dt);

            std::stringstream ss_hydro;
            ss_hydro << ">> HYDROKINETICS: " << callsigns[i] << " Drag Coeff Cd = " 
                     << std::fixed << std::setprecision(4) << fleet_states[i].current_drag_coeff 
                     << " | Position: (" << fleet_states[i].position.x << ", " << fleet_states[i].position.y << ") NM";
            black_box.capture_log_line(ss_hydro.str());

            // ADVANCED STAGE 2: Feed output data arrays into the IMM-UKF Filter tracking nodes
            trackers[i].process_radar_sweep(fleet_states[i].position.x, adaptive_dt);

            // ADVANCED STAGE 3: Structural perimeter violations check via Point-In-Polygon lines
            std::string breached_zone;
            bool inside_perimeter = airspace_control.check_perimeter_penetration(callsigns[i], fleet_states[i].position, breached_zone);
            
            if (inside_perimeter && breached_zone == "R_2508_MIL") {
                std::stringstream ss_alert;
                ss_alert << "🚨 [AIR DEFENSE ALERT]: Target " << callsigns[i] << " breached military restricted airspace " << breached_zone << "!";
                black_box.capture_log_line(ss_alert.str());
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    black_box.capture_log_line("\n--- [AIRPORT RUNWAY OPERATIONS TERMINAL PROCESSING] ---");
    black_box.capture_log_line("🟢 [TOWER]: UAL104 cleared to land. Runway 09R configuration open.");
    black_box.capture_log_line("🟢 [TOWER]: AAL892 cleared to land. Runway 09R configuration open.");

    // The LogExporter destructure block will execute automatically here upon main function exit
    return 0;
}
