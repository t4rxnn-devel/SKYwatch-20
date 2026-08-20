-- SkyWatch-20 ICAO Doc 9855 Compliant Black-Box Incident Recorder Schema
-- Purpose: Set up highly optimized database indexes to log tracking tracks for future investigation

CREATE TABLE IF NOT EXISTS radar_black_box_logs (
    log_id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp_utc TEXT NOT NULL,
    callsign VARCHAR(10) NOT NULL,
    pos_x_nm REAL NOT NULL,
    pos_y_nm REAL NOT NULL,
    altitude_ft REAL NOT NULL,
    velocity_knots REAL NOT NULL,
    tcas_violation_status INTEGER DEFAULT 0
);

-- Generate high-speed structural indexing keys
-- This allows investigators to instantly search and reconstruct an entire route history by its callsign
CREATE INDEX IF NOT EXISTS idx_flight_history 
ON radar_black_box_logs (callsign, timestamp_utc);

-- Generate a security-critical lookup key for quickly auditing safety events
CREATE INDEX IF NOT EXISTS idx_safety_faults 
ON radar_black_box_logs (tcas_violation_status) 
WHERE tcas_violation_status = 1;
