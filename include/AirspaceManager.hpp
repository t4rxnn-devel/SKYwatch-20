// ============================================================================
// SkyWatch-20 AIXM 5.1 / ARINC 424 Standard Airspace Management Header
// Architected for Civil Airport Terminal Areas and Military Air Bases
// ============================================================================

#pragma once
#include "Vector3D.hpp"
#include <vector>
#include <string>
#include <queue>

enum class AirspaceClass {
    CLASS_B_TERMINAL,
    RESTRICTED_SUA
};

struct GeometricVolume3D {
    std::vector<std::pair<double, double>> horizontal_polygon_nm;
    double lower_limit_feet;
    double upper_limit_feet;
};

struct AirspaceSector {
    std::string designator_icao;
    AirspaceClass classification;
    GeometricVolume3D geometry;
    size_t current_traffic_load;
    size_t max_capacity_threshold;
};

class AirspaceManager {
private:
    std::vector<AirspaceSector> m_configured_sectors;
    std::queue<std::string> m_runway_landing_queue;

public:
    AirspaceManager() noexcept;
    
    void provision_airport_airspace(const AirspaceSector& sector);
    
    // Ray-casting point-in-polygon perimeter audit loop
    [[nodiscard]] bool check_perimeter_penetration(const std::string& callsign, 
                                                   const Vector3D& position, 
                                                   std::string& sector_breached) const noexcept;
                                                   
    void sequence_arrival_traffic(const std::string& callsign) noexcept;
    void process_runway_clearance() noexcept;
};
