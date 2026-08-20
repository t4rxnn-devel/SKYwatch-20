#pragma once
#include <cstdint>
#include <cstring>

namespace skywatch::tactical {

// STANAG 5516 J3.2 Air Track Message Format (Bit-packed)
#pragma pack(push, 1)
struct Link16_J3_2_AirTrack {
    uint32_t track_number : 19; // 19-bit J-Series Track Number
    uint32_t identity     : 3;  // Pending, Unknown, Assumed Friend, Friend, Neutral, Suspect, Hostile
    uint32_t spatial_x    : 24; // 24-bit East-North Offset
    uint32_t spatial_y    : 24;
    uint32_t altitude     : 12; // 100ft increments (-1000 to +80,000 ft)
    uint32_t speed        : 10; // Knots
    uint32_t heading      : 9;  // 0.703 degree resolution (360 / 512)
};
#pragma pack(pop)

class Link16MessageDecoder {
public:
    static inline Link16_J3_2_AirTrack decode_j3_2(const uint8_t* raw_buffer) {
        Link16_J3_2_AirTrack msg;
        std::memcpy(&msg, raw_buffer, sizeof(Link16_J3_2_AirTrack));
        return msg;
    }
};

} // namespace skywatch::tactical
