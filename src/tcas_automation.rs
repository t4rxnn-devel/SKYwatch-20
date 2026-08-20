// SkyWatch-20 Traffic Collision Avoidance System (Rust Automation Layer)
use crate::radar_telemetry::FlightTelemetry;

pub struct TcasEngine {
    pub safety_threshold_nm: f64,
}

impl TcasEngine {
    pub fn check_separation(&self, ac1: &FlightTelemetry, ac2: &FlightTelemetry) -> bool {
        // Core 2D Euclidean distance check for quick hazard analysis
        let distance = ((ac1.x_nm - ac2.x_nm).powi(2) + (ac1.y_nm - ac2.y_nm).powi(2)).sqrt();
        
        if distance <= self.safety_threshold_nm {
            println!(
                "⚠️ [TCAS ALERT] Proximity separation broken between {} and {}! Distance: {:.2} NM",
                ac1.callsign, ac2.callsign, distance
            );
            return true; // Conflict detected
        }
        false
    }
}
