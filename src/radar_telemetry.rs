// SkyWatch-20 Radar Telemetry Encoder Module (Rust Engine Layer)

#[derive(Debug, Clone)]
pub struct FlightTelemetry {
    pub callsign: String,
    pub x_nm: f64,
    pub y_nm: f64,
    pub altitude_ft: f64,
    pub speed_knots: f64,
}

impl FlightTelemetry {
    pub fn new(callsign: &str, x: f64, y: f64, alt: f64, speed: f64) -> Self {
        Self {
            callsign: callsign.to_string(),
            x_nm: x,
            y_nm: y,
            altitude_ft: alt,
            speed_knots: speed,
        }
    }

    // Formats and prints telemetry strings for control tower logs
    pub fn print_radar_tag(&self) {
        println!(
            "📡 [TRACK LOG] ID: {} | POS: ({:.2}, {:.2}) NM | ALT: {:.0} FT | SPD: {:.0} KT",
            self.callsign, self.x_nm, self.y_nm, self.altitude_ft, self.speed_knots
        );
    }
}
