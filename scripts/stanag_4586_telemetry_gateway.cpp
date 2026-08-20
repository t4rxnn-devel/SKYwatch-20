#include <iostream>
#include <vector>
#include <cstdint>
#include <cstring>

// STANAG 4586 Message Header Structure (Data Link Interface)
#pragma pack(push, 1)
struct StanagHeader {
    uint8_t sync_byte_1;   // 0xFF
    uint8_t sync_byte_2;   // 0xFE
    uint16_t message_id;   // Msg ID 2000: Vehicle Inertial States
    uint32_t vehicle_id;   // Unique Tail Number / Transponder
    uint16_t payload_len;  // Length in Bytes
    uint32_t time_stamp;   // Time since midnight (ms)
};

struct VehicleInertialStatePayload {
    double latitude;      // Radians
    double longitude;     // Radians
    float altitude_msl;   // Meters
    float u_velocity;     // North velocity (m/s)
    float v_velocity;     // East velocity (m/s)
    float w_velocity;     // Down velocity (m/s)
    float phi;            // Roll angle (rad)
    float theta;          // Pitch angle (rad)
    float psi;            // Heading/Yaw angle (rad)
};
#pragma pack(pop)

extern "C" bool parse_stanag_4586_packet(const uint8_t* buffer, size_t size, VehicleInertialStatePayload* out_state) {
    if (size < sizeof(StanagHeader) + sizeof(VehicleInertialStatePayload)) return false;

    StanagHeader header;
    std::memcpy(&header, buffer, sizeof(StanagHeader));

    if (header.sync_byte_1 != 0xFF || header.sync_byte_2 != 0xFE) return false;
    if (header.message_id == 2000) { // Vehicle State Message
        std::memcpy(out_state, buffer + sizeof(StanagHeader), sizeof(VehicleInertialStatePayload));
        return true;
    }
    return false;
}

int main() {
    std::cout << "[STANAG 4586 Engine] NATO Interoperability Data Link Layer Active.\n";
    return 0;
}
