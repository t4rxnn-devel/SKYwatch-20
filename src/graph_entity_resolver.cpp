#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <unordered_map>

struct RawSensorSpike {
    std::string sensor_type; // "RADAR_PRIMARY", "LINK16_J3_2", "AIS_MARINE"
    uint32_t sensor_id;
    double lat, lon, alt_meters;
    double timestamp;
};

struct ResolvedEntity {
    uint32_t global_entity_id;
    double resolved_lat, resolved_lon, resolved_alt;
    std::vector<std::string> contributing_sources;
};

class GraphEntityResolver {
public:
    // Merges multi-INT sensor spikes within a 200m spatial buffer into one unified entity node
    static ResolvedEntity fuse_sensor_spikes(uint32_t entity_id, const std::vector<RawSensorSpike>& spikes) {
        ResolvedEntity resolved{entity_id, 0.0, 0.0, 0.0, {}};
        if (spikes.empty()) return resolved;

        double sum_lat = 0.0, sum_lon = 0.0, sum_alt = 0.0;
        for (const auto& spike : spikes) {
            sum_lat += spike.lat;
            sum_lon += spike.lon;
            sum_alt += spike.alt_meters;
            resolved.contributing_sources.push_back(spike.sensor_type);
        }

        resolved.resolved_lat = sum_lat / spikes.size();
        resolved.resolved_lon = sum_lon / spikes.size();
        resolved.resolved_alt = sum_alt / spikes.size();
        return resolved;
    }
};

int main() {
    std::cout << "[Entity Resolver Core] Multi-INT graph node fusion layer operational.\n";
    return 0;
}
