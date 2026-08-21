#pragma once

#ifdef __cplusplus
extern "C" {
#endif

    // Opaque structure handles for interop memory buffering
    typedef struct RadarTargetNode {
        uint32_t target_id;
        double latitude;
        double longitude;
        double altitude_ft;
        double velocity_kts;
        double heading_deg;
    } RadarTargetNode;

    // Exported C functions callable from Rust FFI bindings
    __attribute__((visibility("default"))) 
    int evaluate_tcas_separation_risk(const RadarTargetNode* aircraft_a, const RadarTargetNode* aircraft_b);

#ifdef __cplusplus
}
#endif
