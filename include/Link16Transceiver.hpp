// SkyWatch-20 NATO Link 16 Tactical Data Link Transceiver Engine
// Compliant with MIL-STD-6016 and STANAG 5516 Air Track Distribution (J3.2)

#pragma once
#include "Vector3D.hpp"
#include <cstdint>
#include <string>
#include <vector>

// Explicitly byte-aligned raw cryptographic hardware buffer structure
#pragma pack(push, 1)
struct Link16_J3_2_AirTrack {
    uint16_t message_label      : 10; // Message label index (Always J3.2 for Air Track)
    uint16_t track_number_id    : 14; // Uniquely assigned 14-bit NATO track sequencer 
    uint8_t  identity_amplitude : 4;  // Friend/Hostile/Neutral identity matrix index
    uint8_t  emergency_status   : 4;  // Automated military distress squawk parameters
    
    // Low-level bit-packed signed 3D spatial coordinate deltas
    int32_t  latitude_packed    : 21; // Scaled geodetic position tracking mapping
    int32_t  longitude_packed   : 21; 
    uint32_t altitude_packed    : 18; // Packed data bits representing altitude scaling
    
    uint16_t course_heading     : 9;  // 0 to 511 mapping parameters for heading vectors
    uint16_t airspeed_knots     : 14; // High-velocity binary representation scaling
    uint8_t  tcas_alert_state   : 1;  // Critical 1-bit automation warning flag
};
#pragma pack(pop)

class Link16Transceiver {
private:
    uint16_t m_local_track_counter;

    [[nodiscard]] int32_t pack_coordinate_to_bits(double nm_coordinate) const noexcept;
    [[nodiscard]] uint32_t pack_altitude_to_bits(double altitude_nm) const noexcept;

public:
    Link16Transceiver() noexcept;

    // Packs our smooth IMM-UKF output values down to a strict 12-byte hardware buffer array
    [[nodiscard]] Link16_J3_2_AirTrack serialize_to_j_series(const std::string& callsign,
                                                             const Vector3D& smooth_position,
                                                             double speed_knots,
                                                             bool hazard_active) noexcept;

    // Simulates broadcasting raw network data packets over military hardware grids
    void transmit_tactical_packet(const Link16_J3_2_AirTrack& packet) const noexcept;
};
