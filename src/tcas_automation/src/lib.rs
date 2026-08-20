// SkyWatch-20 Hardened Safe Separation Core (Rust Native)
// Compiled as a static library linkable directly by the C++ compiler matrix

pub mod radar_telemetry;
pub mod radar_utils;
pub mod tcas_automation;
pub mod tcas_compliance_node;
pub mod wgs84_gravitational_model;

#[repr(C)]
pub struct AirspaceTarget {
    pub x_nm: f64,
    pub y_nm: f64,
    pub alt_ft: f64,
}

#[no_mangle]
pub extern "C" fn evaluate_loss_of_separation(
    ac1_x: f64, ac1_y: f64, ac1_alt: f64,
    ac2_x: f64, ac2_y: f64, ac2_alt: f64,
    min_horizontal_nm: f64,
    min_vertical_ft: f64,
) -> bool {
    let dx = ac1_x - ac2_x;
    let dy = ac1_y - ac2_y;
    let horizontal_distance = (dx * dx + dy * dy).sqrt();
    let vertical_distance = (ac1_alt - ac2_alt).abs();

    // Trigger true boolean alert flag if perimeter is violated
    horizontal_distance < min_horizontal_nm && vertical_distance < min_vertical_ft
}
