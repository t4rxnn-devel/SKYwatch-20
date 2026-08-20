#include "Link16Transceiver.hpp"
#include <iostream>
#include <iomanip>
#include <cstring>

Link16Transceiver::Link16Transceiver() noexcept : m_local_track_counter(1024) {}

int32_t Link16Transceiver::pack_coordinate_to_bits(double nm_coordinate) const noexcept {
    // Standard tactical scale factor mapping: convert coordinate into 21-bit signed bounds
    // Bounded between standard radar ranges (-180 to +180 degrees map equivalents)
    double scaled_val = (nm_coordinate / 180.0) * 1048575.0;
    return static_cast<int32_t>(scaled_val) & 0x1FFFFF;
}

uint32_t Link16Transceiver::pack_altitude_to_bits(double altitude_nm) const noexcept {
    double altitude_feet = altitude_nm * 6076.12;
    // Compress up to Flight Level 1000 into an exact 18-bit block wrapper
    return static_cast<uint32_t>(altitude_feet / 25.0) & 0x3FFFF;
}

Link16_J3_2_AirTrack Link16Transceiver::serialize_to_j_series(const std::string& callsign,
                                                             const Vector3D& smooth_position,
                                                             double speed_knots,
                                                             bool hazard_active) noexcept {
    Link16_J3_2_AirTrack track_packet;
    std::memset(&track_packet, 0, sizeof(Link16_J3_2_AirTrack));

    // J3.2 NATO standard label designation sequence ID
    track_packet.message_label = 3; 
    track_packet.track_number_id = ++m_local_track_counter;
    
    // Threat Evaluation Mapping (If target is designated an intruder like SU-57, tag as hostile)
    if (callsign == "SU-57") {
        track_packet.identity_amplitude = 2; // 2 = Assured Hostile threat vector index
        track_packet.emergency_status = 0;
    } else {
        track_packet.identity_amplitude = 1; // 1 = Assured Friend air profile path
        track_packet.emergency_status = 0;
    }

    // Apply strict hardware bit-packing mathematical scalers
    track_packet.latitude_packed = pack_coordinate_to_bits(smooth_position.x);
    track_packet.longitude_packed = pack_coordinate_to_bits(smooth_position.y);
    track_packet.altitude_packed = pack_altitude_to_bits(smooth_position.z);
    
    // Scale tracking orientation heading fields [0 to 360 mapped across 9 bits]
    track_packet.course_heading = static_cast<uint16_t>((3.14159 / 2.0) * (511.0 / (2.0 * 3.14159))) & 0x1FF;
    track_packet.airspeed_knots = static_cast<uint16_t>(speed_knots) & 0x3FFF;
    track_packet.tcas_alert_state = hazard_active ? 1 : 0;

    return track_packet;
}

void Link16Transceiver::transmit_tactical_packet(const Link16_J3_2_AirTrack& packet) const noexcept {
    std::cout (&packet);
    std::cout << "    • Cryptographic Payload Hex Dump: [ ";
    for (size_t i = 0; i < sizeof(Link16_J3_2_AirTrack); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte_ptr[i]) << " ";
    }
    std::cout << std::dec << "]\n";
}
