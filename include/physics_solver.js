// SkyWatch-20 4th-Order Runge-Kutta Precision Integration Core Module
import * as THREE from 'https://cloudflare.com';

// Internal utility function evaluating acceleration fields
function getAccelerationField(position, velocity, targetSpeed, storms) {
    let accel = new THREE.Vector3(0, 0, 0);
    
    // 1. Guidance vector attraction pulling targets back toward radar origin
    let targetHeading = new THREE.Vector3(0, 0, 0).sub(position);
    targetHeading.y = 0; 
    targetHeading.normalize();

    let thrustForce = 0.45;
    let currentSpeed = velocity.length();
    if (currentSpeed < targetSpeed) {
        accel.addScaledVector(targetHeading, thrustForce);
    }

    // 2. Aerodynamic parasitic drag coefficient equations
    let dragCoefficient = 0.12;
    accel.addScaledVector(velocity, -dragCoefficient * currentSpeed);

    // 3. Storm localized turbulence injections
    storms.forEach(storm => {
        let dist = position.distanceTo(storm.center);
        if (dist <= storm.radius) {
            let intensityFactor = Math.pow(1.0 - (dist / storm.radius), 2);
            accel.addScaledVector(storm.shearDirection, storm.windForce * intensityFactor);
        }
    });

    return accel;
}

// Global module function exported to step flight profiles forward via true RK4 steps
export function computeRk4Step(ac, storms, dt) {
    let p0 = ac.pos.clone(); 
    let v0 = ac.vel.clone();

    // K1 derivative evaluation
    let a1 = getAccelerationField(p0, v0, ac.targetSpeed, storms); 
    let v1 = v0.clone();

    // K2 derivative evaluation
    let p2 = p0.clone().addScaledVector(v1, dt * 0.5); 
    let v2 = v0.clone().addScaledVector(a1, dt * 0.5); 
    let a2 = getAccelerationField(p2, v2, ac.targetSpeed, storms);

    // K3 derivative evaluation
    let p3 = p0.clone().addScaledVector(v2, dt * 0.5); 
    let v3 = v0.clone().addScaledVector(a2, dt * 0.5); 
    let a3 = getAccelerationField(p3, v3, ac.targetSpeed, storms);

    // K4 derivative evaluation
    let p4 = p0.clone().addScaledVector(v3, dt); 
    let v4 = v0.clone().addScaledVector(a3, dt); 
    let a4 = getAccelerationField(p4, v4, ac.targetSpeed, storms);

    // Core weighted step updating flight values directly inside memory matrices
    ac.pos.addScaledVector(v1.addScaledVector(v2, 2.0).addScaledVector(v3, 2.0).add(v4), dt / 6.0);
    ac.vel.addScaledVector(a1.addScaledVector(a2, 2.0).addScaledVector(a3, 2.0).add(a4), dt / 6.0);
}
