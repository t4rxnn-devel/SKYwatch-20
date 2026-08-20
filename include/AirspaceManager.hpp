// SkyWatch-20 AIXM 5.1 / ARINC 424 Standard Airspace Management Engine
// Architected for Civil Airport Terminal Areas and Military Air Bases

#pragma once
#include "Vector3D.hpp"
#include <vector>
#include <string>
#include <queue>

// Matches standard AIXM CodeAirspaceType definitions
enum class AirspaceClass {
    CLASS_A_ENROUTE, // High-altitude strict IFR controls (18k+ FT)
    CLASS_B_TERMINAL,// Busy commercial airport core layers (Surface to 10k FT)
    RESTRICTED_SUA,  // Special Use Airspace (Military training/danger zones)
    APPROACH_CORRIDOR// Dedicated arrival/departure transition tunnels
};

struct GeometricVolume3D {
    std::vector<std::pair<double, double>> horizontal_polygon_nm;
    double lower_limit_feet;
    double upper_limit_feet;
};

struct AirspaceSector {
    std::string designator_icao; // e.g., "KORD_APP_NW"
    AirspaceClass classification;
    GeometricVolume3D geometry;
    size_t current_traffic_load;
    size_t max_capacity_threshold;
};

class AirspaceManager {
private:
    std::vector<AirspaceSector> m_configured_sectors;
    std::queue<std::string> m_runway_landing_queue; // Automated sequencing stack

public:
    AirspaceManager() noexcept;

    // Registers structural map definitions into memory
    void provision_airport_airspace(const AirspaceSector& sector);

    // Audits aircraft position vectors against ARINC restricted boundaries
    [[nodiscard]] bool check_perimeter_penetration(const std::string& callsign, 
                                                   const Vector3D& position, 
                                                   std::string& sector_breached) const noexcept;

    // Arranges arrival flights using real airport sequencing guidelines
    void sequence_arrival_traffic(const std::string& callsign) noexcept;
    
    void process_runway_clearance() noexcept;
};
