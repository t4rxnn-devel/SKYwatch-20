#include "TimeSyncEngine.hpp"
#include <iostream>
#include <cmath>

TimeSyncEngine::TimeSyncEngine() noexcept 
    : m_gps_utc_leap_seconds(18), // Current established GPS-UTC leap second offset parameter
      m_max_allowable_drift_ms(5.0) {} // FAA strict maximum tolerance boundary limit

SyncStatus TimeSyncEngine::audit_system_chronology(uint64_t network_master_time_ms) const noexcept {
    SyncStatus status;
    
    // Fetch local hardware system clock parameters via high-precision clock intervals
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t local_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    // Compute absolute synchronization time differential delta values
    status.clock_drift_ms = std::abs(static_cast<double>(local_time_ms) - static_cast<double>(network_master_time_ms));
    status.current_leap_seconds = m_gps_utc_leap_seconds;
    status.gps_epoch_seconds = local_time_ms / 1000;
    
    // Map conversion calculation adding standard leap deviations back into tracking logs
    status.utc_epoch_seconds = status.gps_epoch_seconds - m_gps_utc_leap_seconds;
    status.is_synchronized = (status.clock_drift_ms <= m_max_allowable_drift_ms);

    if (!status.is_synchronized) {
        std::cout << "🚨 [CHRONO CRITICAL ALERT]: Radar clock synchronization drift detected!\n";
        std::cout << "                          Variance: " << status.clock_drift_ms 
                  << " ms (Max Limit: " << m_max_allowable_drift_ms << " ms)\n";
    } else {
        std::cout << "🛰️ [CHRONO]: Network time master lock stabilized. Leap second delta compensated: -" 
                  << m_gps_utc_leap_seconds << "s.\n";
    }

    return status;
}

double TimeSyncEngine::compensate_timestamp(double raw_timestamp_seconds) const noexcept {
    // Strips away leap shifts before position prediction steps run inside the solver
    return raw_timestamp_seconds - static_cast<double>(m_gps_utc_leap_seconds);
}
