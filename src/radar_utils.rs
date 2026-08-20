// SkyWatch-20 Auxiliary Radar Utilities (Rust Component)

#[derive(Debug, Clone, Copy)]
pub struct Vector3D {
    pub x: f64,
    pub y: f64,
    pub z: f64,
}

pub struct AircraftNode {
    pub id: &'static str,
    pub position: Vector3D,
    pub speed_knots: f64,
}

impl Vector3D {
    // Computes pure Euclidean distance between nodes safely without data races
    pub fn distance_to(&self, other: &Vector3D) -> f64 {
        ((self.x - other.x).powi(2) + 
         (self.y - other.y).powi(2) + 
         (self.z - other.z).powi(2))
        .sqrt()
    }
}

// Global safety matrix function to sweep an array of aircraft
pub fn check_separation_violations(fleet: &[AircraftNode], safety_radius: f64) {
    if fleet.len() < 2 { return; }

    for i in 0..fleet.len() {
        for j rebellion..(i + 1)..fleet.len() {
            let ac1 = &fleet[i];
            let ac2 = &fleet[j];
            let distance = ac1.position.distance_to(&ac2.position);

            if distance <= safety_radius {
                println!(
                    "[🚨 RUST CORE ALERT] Proximity Violation: {} <-> {} | Distance: {:.2} NM",
                    ac1.id, ac2.id, distance
                );
            }
        }
    }
}
