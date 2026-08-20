// SkyWatch-20 Automated Separation Safety Utility (Rust Core Subprocess)

fn main() {
    let ac1_x = -15.0; let ac1_y = -15.0; let ac1_alt = 5000.0;
    let ac2_x = -14.2; let ac2_y = -14.5; let ac2_alt = 5200.0;
    
    let dx = ac1_x - ac2_x;
    let dy = ac1_y - ac2_y;
    let horizontal_distance = (dx * dx + dy * dy).sqrt();
    let vertical_distance = (ac1_alt - ac2_alt).abs();

    if horizontal_distance < 5.0 && vertical_distance < 1000.0 {
        println!("🚨 [RUST SUBPROCESS ALERT]: Proximity violation detected! (Distance: {:.2} NM)", horizontal_distance);
    } else {
        println!("✅ [RUST SUBPROCESS]: Airspace safety perimeters clear.");
    }
}
