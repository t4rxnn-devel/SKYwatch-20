// RTCA DO-178C Level A Compliance Assurance Module
pub struct DO178CSafetyMonitor;

#[derive(Debug, PartialEq)]
pub enum SafetyStatus {
    Nominal,
    LossOfSeparationWarning,
    CriticalAltitudeViolation,
}

impl DO178CSafetyMonitor {
    /// Evaluates 3D spatial separation between target pairs against DO-178C safety buffers
    pub fn verify_separation_buffer(
        pos_a: (f64, f64, f64),
        pos_b: (f64, f64, f64),
        min_horizontal_meters: f64,
        min_vertical_meters: f64,
    ) -> SafetyStatus {
        let dx = pos_a.0 - pos_b.0;
        let dy = pos_a.1 - pos_b.1;
        let dz = (pos_a.2 - pos_b.2).abs();

        let horizontal_dist = (dx * dx + dy * dy).sqrt();

        if horizontal_dist < min_horizontal_meters && dz < min_vertical_meters {
            return SafetyStatus::LossOfSeparationWarning;
        }

        if pos_a.2 < 100.0 || pos_b.2 < 100.0 {
            return SafetyStatus::CriticalAltitudeViolation;
        }

        SafetyStatus::Nominal
    }
}
