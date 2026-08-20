/**
 * Hermite Spline Extrapolation for 60 FPS Target Interpolation
 */
class VelocityExtrapolator {
    constructor() {
        this.states = new Map();
    }

    updateState(id, x, y, z, vx, vy, vz, timestamp) {
        this.states.set(id, { x, y, z, vx, vy, vz, timestamp });
    }

    getExtrapolatedPosition(id, renderTime) {
        const state = this.states.get(id);
        if (!state) return null;

        const dt = Math.max(0, (renderTime - state.timestamp) / 1000.0);
        
        // Kinematic Taylor Expansion: x_render = x0 + v*dt
        return {
            x: state.x + state.vx * dt,
            y: state.y + state.vy * dt,
            z: state.z + state.vz * dt
        };
    }
}

if (typeof module !== 'undefined') {
    module.exports = { VelocityExtrapolator };
}
