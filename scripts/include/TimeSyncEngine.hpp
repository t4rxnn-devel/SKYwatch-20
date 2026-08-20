// SkyWatch-20 High-Integrity GNSS / UTC Chronological Synchronization Core
// Compliant with ICAO Doc 9855 and FAA Air Traffic Sync Directives

#pragma once
#include <chrono>
#include <string>

struct SyncStatus {
    uint64_t gps_epoch_seconds;
    uint64_t utc_epoch_seconds;
    int current_leap_seconds;
    double clock_drift_ms;
    bool is_synchronized;
};

class TimeSyncEngine {
private:
    // Standard accumulated offset tracking between GPS Atomic time and UTC
    int m_gps_utc_leap_seconds;
    double m_max_allowable_drift_ms;

public:
    TimeSyncEngine() noexcept;

    // Evaluates local radar computer clock parameters against master GNSS atomic clock pings
    [[nodiscard]] SyncStatus audit_system_chronology(uint64_t network_master_time_ms) const noexcept;

    // Adjusts raw tracking timestamps to prevent velocity vector calculation drift
    [[nodiscard]] double compensate_timestamp(double raw_timestamp_seconds) const noexcept;
};
