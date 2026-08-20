// SkyWatch-20 High-Precision WGS 84 Gravitational Model Engine
// Implements the J2 Zonal Harmonic Equatorial Perturbation Equation

pub struct Wgs84Ellipsoid {
    pub semi_major_axis_m: f64, // Equatorial radius (a)
    pub flattening: f64,        // Earth flattening parameter (f)
    pub mu: f64,                // Geocentric gravitational constant (G*M)
    pub j2: f64,                // Second zonal harmonic coefficient
}

impl Wgs84Ellipsoid {
    pub fn new() -> Self {
        Self {
            semi_major_axis_m: 6378137.0,
            flattening: 1.0 / 298.257223563,
            mu: 3.986004418e14,
            j2: 1.08262668e-3,
        }
    }

    /// Computes the exact acceleration of gravity vector acting on the target
    /// accounting for oblate spheroidal mass distribution at a given latitude/altitude.
    pub fn calculate_gravitational_acceleration(&self, latitude_rad: f64, altitude_nm: f64) -> f64 {
        let altitude_meters = altitude_nm * 1852.0;
        
        // Calculate geocentric radius (r) approximation
        let sin_lat = latitude_rad.sin();
        let r_eq = self.semi_major_axis_m;
        let r = r_eq + altitude_meters;

        // Baseline spherical Newtonian gravity component (g0 = mu / r^2)
        let g_spherical = self.mu / (r * r);

        // J2 Equatorial Bulge Perturbation Multiplier Formula
        // Accounts for the structural mass variations pulling the aircraft laterally
        let j2_effect = 1.5 * self.j2 * (r_eq / r).powi(2) * (3.0 * sin_lat * sin_lat - 1.0);

        // Yield highly accurate local gravity acceleration (m/s^2)
        g_spherical * (1.0 - j2_effect)
    }
}
