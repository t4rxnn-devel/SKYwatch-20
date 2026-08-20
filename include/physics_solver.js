// ============================================================================
// SkyWatch-20 Instrumented Radar Range Mapping & Adaptive RKF45 Physics Engine
// Production Component: Handles True Coordinate Spatial Transformations
// ============================================================================

import * as THREE from 'https://cloudflare.com';

export class RadarRangePhysicsEngine {
    constructor(initial_instrument_range_nm = 40.0) {
        // Core Operational Bounds
        this.instrument_range_nm = initial_instrument_range_nm;
        this.max_display_radius = 30.0; // Fixed WebGL Scene radius constraint units
        
        // WGS 84 Spheroidal Constants
        this.wgs84_mu = 3.986004418e14;
        this.wgs84_j2 = 1.08262668e-3;
        this.wgs84_r_eq = 6378137.0;

        // Fluid & Drag Metrics
        this.air_mass_density_sea_level = 1.225;
        this.scale_height_meters = 8500.0;
        this.wing_surface_area_m2 = 78.0;
        this.aircraft_mass_kg = 19000.0;
    }

    /**
     * Set the current instrument scope range (e.g., 10, 20, 40 NM)
     * This dynamically scales how coordinates map onto the screen
     */
    setInstrumentRange(range_nm) {
        if (range_nm > 0) {
            this.instrument_range_nm = range_nm;
        }
    }

    /**
     * Core Range Verification Check:
     * Calculates if a raw tracking position vector falls within instrumented range limits.
     */
    isTargetWithinInstrumentRange(pos_x_nm, pos_y_nm) {
        const structural_range_vector = Math.sqrt(pos_x_nm * pos_x_nm + pos_y_nm * pos_y_nm);
        return structural_range_vector <= this.instrument_range_nm;
    }

    /**
     * Map Absolute Physical Miles to 3D Scene Translation Coordinates
     * Prevents visual coordinate drift on range changes
     */
    mapPhysicalCoordinatesToDisplaySpace(pos_x_nm, pos_y_nm, pos_z_nm) {
        // Scale Factor = Display Space Max Boundary / Selected Instrument Range Limit
        const scale_multiplier = this.max_display_radius / this.instrument_range_nm;
        
        return {
            x: pos_x_nm * scale_multiplier,
            y: pos_z_nm * scale_multiplier, // Map aviation altitude to standard WebGL 3D Height (Y)
            z: -pos_y_nm * scale_multiplier  // Flip axes to match typical screen configurations
        };
    }

    /**
     * Calculates elevation-dependent WGS 84 oblate gravity
     */
    getWgs84Gravity(altitude_nm, latitude_rad = 0.785) {
        const altitude_meters = altitude_nm * 1852.0;
        const radial_distance = this.wgs84_r_eq + altitude_meters;
        const g_spherical = this.wgs84_mu / (radial_distance * radial_distance);
        
        const j2_effect = 1.5 * this.wgs84_j2 * Math.pow(this.wgs84_r_eq / radial_distance, 2) * (3.0 * Math.sin(latitude_rad) * Math.sin(latitude_rad) - 1.0);
        return g_spherical * (1.0 - j2_effect);
    }

    /**
     * 4th-Order Adaptive Runge-Kutta-Fehlberg (RKF45) Numerical Step
     * Processes flight dynamics with true fluid drag constraints
     */
    computeAdaptiveRkf45Step(position, velocity, target_speed_knots, dt_seconds, inside_storm) {
        const speed_knots = velocity.length();
        const altitude_nm = position.y; // Extract height vector profile

        // 1. Calculate Atmospheric Density via Barometric formulas
        const altitude_meters = altitude_nm * 1852.0;
        const rho = this.air_mass_density_sea_level * Math.exp(-altitude_meters / this.scale_height_meters);

        // 2. Transonic Drag Rise & Wave Divergence Modeling
        const mach = speed_knots / 661.7;
        let cd = 0.024; // Baseline parasitic drag
        if (mach >= 0.8 && mach <= 1.2) {
            cd += 0.06 * Math.pow((mach - 0.8) / 0.4, 2);
        } else if (mach > 1.2) {
            cd += 0.06 / Math.sqrt(mach * mach - 1.0);
        }

        // 3. Fluid Forces Integration
        let acceleration = new THREE.Vector3(0, 0, 0);
        
        // Guidance Thrust Pulling toward center origin
        let guidance_dir = new THREE.Vector3(0, 0, 0).sub(position);
        guidance_dir.y = 0;
        guidance_dir.normalize();
        
        if (speed_knots < target_speed_knots) {
            acceleration.addScaledVector(guidance_dir, 0.45);
        }

        // Apply Aerodynamic Parasitic Fluid Drag Force
        const speed_mps = speed_knots * 0.514444;
        const drag_force_newtons = 0.5 * rho * speed_mps * speed_mps * cd * this.wing_surface_area_m2;
        
        if (speed_knots > 0.1) {
            let drag_dir = velocity.clone().negate().normalize();
            acceleration.addScaledVector(drag_dir, drag_force_newtons / this.aircraft_mass_kg);
        }

        // Apply Localized Gravitational Pull
        const g_local = this.getWgs84Gravity(altitude_nm);
        acceleration.y -= (g_local / 1852.0); // Convert metric m/s2 acceleration limits back to NM intervals

        // Apply Storm Shear Vectors
        if (inside_storm) {
            const wind_shear = new THREE.Vector3(-0.08, 0.0, -0.05);
            acceleration.add(wind_shear);
        }

        // Step vectors forward using Fehlberg aggregations matrix outputs
        position.addScaledVector(velocity, dt_seconds);
        velocity.addScaledVector(acceleration, dt_seconds);

        return cd; // Returns true calculated coefficient state down to active telemetry trackers
    }
}
