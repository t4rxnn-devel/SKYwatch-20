// SkyWatch-20 Automated Separation Safety Utility (Rust Core)
// Compares flight nodes against strict 5.0 NM horizontal and 1,000 FT vertical limits

#[derive(Debug, Clone)]
pub struct AirspaceTarget {
    pub callsign: String,
    pub x_nm: f64,
    pub y_nm: f64,
    pub alt_ft: f64,
}

pub struct TcasSafetyAuditor {
    pub min_horizontal_nm: f64,
    pub min_vertical_ft: f64,
}

impl TcasSafetyAuditor {
    pub fn new() -> Self {
        Self {
            min_horizontal_nm: 5.0,
            min_vertical_ft: 1000.0,
        }
    }

    /// Evaluates proximity vectors between two active aircraft nodes
    pub fn evaluate_loss_of_separation(&self, ac1: &AirspaceTarget, ac2: &AirspaceTarget) -> bool {
        // Calculate 2D horizontal Euclidean distance
        let dx = ac1.x_nm - ac2.x_nm;
        let dy = ac1.y_nm - ac2.y_nm;
        let horizontal_distance = (dx * dx + dy * dy).sqrt();

        // Calculate absolute vertical separation
        let vertical_distance = (ac1.alt_ft - ac2.alt_ft).abs();

        // Trigger safety alert if BOTH thresholds are violated simultaneously
        if horizontal_distance < self.min_horizontal_nm && vertical_distance < self.min_vertical_ft {
            println!(
                "🚨 [RUST SAFETY CRITICAL]: Loss of separation between {} and {}! (H: {:.2} NM, V: {:.0} FT)",
                ac1.callsign, ac2.callsign, horizontal_distance, vertical_distance
            );
            return true;
        }
        false
    }
}
