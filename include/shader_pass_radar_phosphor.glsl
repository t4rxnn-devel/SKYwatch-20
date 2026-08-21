// GLSL Fragment Shader: Phosphor Decay & Sweep Line Radar Simulation
precision mediump float;

varying vec2 v_uv;
uniform float u_time;
uniform vec2 u_resolution;

void main() {
    vec2 st = gl_FragCoord.xy / u_resolution.xy;
    vec2 center = vec2(0.5, 0.5);
    float dist = distance(st, center);

    // Dynamic Rotating Radar Sweep Line
    float angle = atan(st.y - center.y, st.x - center.x);
    float sweep_angle = mod(u_time * 2.0, 6.28318) - 3.14159;
    float diff = mod(angle - sweep_angle + 6.28318, 6.28318);

    // Phosphor green decay trail
    float alpha = exp(-diff * 3.0) * step(dist, 0.48);
    
    // Phosphor Green Color Palette
    vec3 color = vec3(0.0, 0.95, 0.3) * alpha;
    
    // Concentric Range Rings (0.1, 0.2, 0.3, 0.4 radius)
    float ring = smoothstep(0.002, 0.0, abs(fract(dist * 10.0 - 0.05) - 0.05));
    color += vec3(0.0, 0.4, 0.15) * ring * step(dist, 0.48);

    gl_FragColor = vec4(color, alpha);
}
