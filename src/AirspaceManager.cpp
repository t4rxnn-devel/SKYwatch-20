// ============================================================================
// SkyWatch-20 AIXM 5.1 / ARINC 424 Standard Airspace Management Engine
// Architected for Civil Airport Terminal Areas and Military Air Bases
// ============================================================================

#include "../include/AirspaceManager.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <iomanip>

AirspaceManager::AirspaceManager() noexcept {
    // 1. Provision a Standard Class B Commercial Terminal Core Zone (e.g., KORD)
    AirspaceSector kord_core;
    kord_core.designator_icao = "KORD_CLASS_B";
    kord_core.classification = AirspaceClass::CLASS_B_TERMINAL;
    kord_core.geometry.lower_limit_feet = 0.0;
    kord_core.geometry.upper_limit_feet = 10000.0;
    
    // Multi-vertex bounding polygon mapping terminal corridor boundaries (10 NM Radius)
    kord_core.geometry.horizontal_polygon_nm = { 
        {-10.0, -10.0}, {10.0, -10.0}, {10.0, 10.0}, {-10.0, 10.0} 
    };
    kord_core.current_traffic_load = 0;
    kord_core.max_capacity_threshold = 15;
    provision_airport_airspace(kord_core);

    // 2. Provision a Specialized Military Special Use Airspace (SUA Restricted Zone)
    AirspaceSector red_flag_zone;
    red_flag_zone.designator_icao = "R_2508_MIL";
    red_flag_zone.classification = AirspaceClass::RESTRICTED_SUA;
    red_flag_zone.geometry.lower_limit_feet = 2000.0;
    red_flag_zone.geometry.upper_limit_feet = 60000.0;
    
    // High-altitude danger boundary corridor
    red_flag_zone.geometry.horizontal_polygon_nm = { 
        {12.0, 12.0}, {24.0, 12.0}, {24.0, 24.0}, {12.0, 24.0} 
    };
    red_flag_zone.current_traffic_load = 0;
    red_flag_zone.max_capacity_threshold = 5;
    provision_airport_airspace(red_flag_zone);
}

void AirspaceManager::provision_airport_airspace(const AirspaceSector& sector) {
    m_configured_sectors.push_back(sector);
}

bool AirspaceManager::check_perimeter_penetration(const std::string& callsign, 
                                                   const Vector3D& position, 
                                                   std::string& sector_breached) const noexcept {
    // Convert current vertical metric from Nautical Miles to standard aviation Feet
    double altitude_feet = position.z * 6076.11549; // Precise ICAO NM-to-Feet scalar conversion

    for (const auto& sector : m_configured_sectors) {
        // Vertical airspace ceiling and floor validation bounds checks first
        if (altitude_feet < sector.geometry.lower_limit_feet || 
            altitude_feet > sector.geometry.upper_limit_feet) {
            continue;
        }

        // Ray-Casting Point-in-Polygon (PIP) Algorithm to verify 2D coordinate map parameters
        bool inside_polygon = false;
        const auto& poly = sector.geometry.horizontal_polygon_nm;
        size_t num_vertices = poly.size();
        
        for (size_t i = 0, j = num_vertices - 1; i < num_vertices; j = i++) {
            // Check crosshair vector line intersection boundaries
            if (((poly[i].second > position.y) != (poly[j].second > position.y)) &&
                (position.x < (poly[j].x - poly[i].x) * (position.y - poly[i].second) / 
                (poly[j].second - poly[i].second) + poly[i].x)) {
                inside_polygon = !inside_polygon;
            }
        }

        if (inside_polygon) {
            sector_breached = sector.designator_icao;
            if (sector.classification == AirspaceClass::RESTRICTED_SUA) {
                std::cout << "🚨 [AIR DEFENSE ALERT]: Target " << callsign 
                          << " breached military restricted airspace " << sector_breached << "!\n";
            } else {
                std::cout << "📡 [ATC INFO]: Handing over " << callsign << " to sector " << sector_breached << ".\n";
            }
            return true;
        }
    }
    return false;
}

void AirspaceManager::sequence_arrival_traffic(const std::string& callsign) noexcept {
    m_runway_landing_queue.push(callsign);
    std::cout << "🛫 [SEQUENCER]: " << callsign << " placed in landing sequence queue. Position: " 
              << m_runway_landing_queue.size() << "\n";
}

void AirspaceManager::process_runway_clearance() noexcept {
    if (m_runway_landing_queue.empty()) return;

    std::string cleared_flight = m_runway_landing_queue.front();
    m_runway_landing_queue.pop();
    std::cout << "🟢 [TOWER]: " << cleared_flight << " cleared to land. Runway 09R configuration open.\n";
}
