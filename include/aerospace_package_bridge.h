// SkyWatch-20 Unified Aerospace Package Linkage Bridge (C-FFI Standard)
#pragma once
#include <stdbool>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Strict byte-aligned NATO Link 16 J3.2 Tactical Tracking Packet Structure
#pragma pack(push, 1)
typedef struct {
    uint16_t message_label      : 10; // J3.2 Air Track Standard
    uint16_t track_number_id    : 14; // NATO assigned ID identifier
    uint8_t  identity_amplitude : 4;  // Combat identity index (1=Friend, 2=Hostile)
    uint8_t  emergency_status   : 4;  
    int32_t  latitude_packed    : 21; // Signed geodetic position metrics
    int32_t  longitude_packed   : 21; 
    uint32_t altitude_packed    : 18; // Packed elevation tracking data bits
    uint16_t course_heading     : 9;  
    uint16_t airspeed_knots     : 14; 
    uint8_t  tcas_alert_state   : 1;  
} Link16_J3_2_Packet;
#pragma pack(pop)

// Rust Shared Library Function Declarations
bool rust_evaluate_rtca_do178c_separation(
    double ac1_x, double ac1_y, double ac1_alt,
    double ac2_x, double ac2_y, double ac2_alt,
    double min_h_nm, double min_v_ft
);

Link16_J3_2_Packet rust_serialize_nato_link16(
    const char* callsign, double x, double y, double z_alt, double speed, bool alert
);

// Mojo Shared Library Function Declarations
void mojo_parallel_spatial_hash_sweep(int structural_batch_size, double radius_threshold);

#ifdef __cplusplus
}
#endif
