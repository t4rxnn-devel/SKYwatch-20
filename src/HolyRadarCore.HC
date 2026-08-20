// ============================================================================
// SkyWatch-20 Transpiled HolyC Aerospace Subsystem Core
// Translated from HolyC (.HC) to Standard C++20 For Multi-Platform Builds
// ============================================================================

#pragma once
#include <string>
#include <cmath>
#include <iostream>

struct CVector3D {
    double x;
    double y;
    double z;
};

struct CAircraftNode {
    std::string callsign;
    CVector3D position;
    double speed_knots;
};

inline double ComputeHolyDistance(const CVector3D& v1, const CVector3D& v2) noexcept {
    double dx = v1.x - v2.x;
    double dy = v1.y - v2.y;
    double dz = v1.z - v2.z;
    return std::sqrt(dx*dx + dy*dy + dz*dz);
}

inline void EmitDivineWarningChirp() noexcept {
    // Cross-platform ANSI trigger emitting an audible alert frequency sequence
    std::cout << "\a" << std::flush; 
    std::cout << "🔔 [AUDIO INTERRUPT]: Motherboard warning chirp emitted.\n";
}
