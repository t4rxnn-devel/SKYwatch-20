#include "skywatch_ffi.hpp"
#include <cmath>
#include <algorithm>

extern "C" {
    int evaluate_tcas_separation_risk(const RadarTargetNode* aircraft_a, const RadarTargetNode* aircraft_b) {
        if (!aircraft_a || !aircraft_b) return -1;

        // Compute horizontal distance approximation (NM) and vertical delta (ft)
        double d_lat = aircraft_a->latitude - aircraft_b->latitude;
        double d_lon = aircraft_a->longitude - aircraft_b->longitude;
        double horizontal_nm = std::sqrt(d_lat * d_lat + d_lon * d_lon) * 60.0; // Rough conversion
        double vertical_ft = std::abs(aircraft_a->altitude_ft - aircraft_b->altitude_ft);

        // RA (Resolution Advisory) trigger thresholds: < 1.0 NM horizontal and < 1000 ft vertical
        if (horizontal_nm < 1.0 && vertical_ft < 1000.0) {
            return 2; // Critical Collision Warning (RA)
        } else if (horizontal_nm < 3.0 && vertical_ft < 1200.0) {
            return 1; // Traffic Advisory (TA)
        }
        
        return 0; // Clear
    }
}
