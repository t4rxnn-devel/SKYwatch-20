/* SkyWatch-20 High-Fidelity Tactical Radar Shading Engine */
/* Language: GLSL (OpenGL Shading Language / WebGL Fragment) */

precision mediump float;

// Uniform variables passed directly from the hardware render clock loop
uniform vec2  u_resolution;  // Screen dimensions vector (Width, Height)
uniform float u_sweep_angle; // Instantaneous angle of the rotating scan beam
uniform vec2  u_storm_center;// Center coordinate vector of weather masses

void main() {
    // 1. Normalize pixel coordinates to map from a center anchor point (-1.0 to +1.0)
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_resolution) / min(u_resolution.x, u_resolution.y);
    float distance_from_center = length(uv);

    // Hard clip rendering space to an outer 3D compass ring radius perimeter boundary
    if (distance_from_center > 0.95) {
        discard; // Instantly drops pixel processing outside tactical radar boundaries
    }

    // 2. Calculate Angular Delta relative to the rotating scanner beam
    float pixel_angle = atan(uv.y, uv.x);
    if (pixel_angle < 0.0) pixel_angle += 2.0 * 3.1415926535;

    float angle_diff = u_sweep_angle - pixel_angle;
    if (angle_diff < 0.0) angle_diff += 2.0 * 3.1415926535;

    // 3. Mathematical Phosphor Decay Logic
    // Compute intense glow at the beam front, transitioning to an exponential fade trail
    float trace_intensity = 0.0;
    if (angle_diff < 0.15) {
        trace_intensity = 1.0 - (angle_diff / 0.15); // Intense Leading Edge Scan
    } else {
        trace_intensity = 0.25 * exp(-1.2 * (angle_diff - 0.15)); // Trailing Phosphorus Decay Curve
    }

    // 4. Base Tactical Color Palette Selection (Classic Military Dark Green Grid)
    vec3 radar_grid_color = vec3(0.0, 0.45, 0.12) * trace_intensity;

    // 5. Doppler Weather Matrix Overlay Integration
    // If pixel steps inside the storm coordinates bounds, layer a dense orange tint
    float distance_to_storm = distance(uv, u_storm_center);
    if (distance_to_storm < 0.35) {
        float storm_density = (1.0 - (distance_to_storm / 0.35)) * 0.4;
        radar_grid_color += vec3(0.85, 0.22, 0.0) * storm_density; // Amber/Orange Doppler Echo
    }

    // Add subtle ambient sonar glow to center crosshairs
    radar_grid_color += vec3(0.0, 0.15, 0.05) * (1.0 - distance_from_center);

    gl_FragColor = vec4(radar_grid_color, 1.0);
}
