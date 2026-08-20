/**
 * Cubic Hermite Spline Extrapolator for Zero-Latency 60 FPS Tactical Rendering
 */
class VelocityExtrapolator {
    constructor() {
        this.states = new Map();
    }

    /**
     * Updates target state buffer maintaining past kinematic history.
     */
    updateState(id, x, y, z, vx, vy, vz, timestamp) {
        const prev = this.states.get(id);
        
        if (!prev) {
            this.states.set(id, {
                x0: x, y0: y, z0: z,
                x1: x, y1: y, z1: z,
                vx0: vx, vy0: vy, vz0: vz,
                vx1: vx, vy1: vy, vz1: vz,
                t0: timestamp, t1: timestamp
            });
            return;
        }

        // Shift state history window for cubic interpolation
        this.states.set(id, {
            x0: prev.x1, y0: prev.y1, z0: prev.z1,
            x1: x, y1: y, z1: z,
            vx0: prev.vx1, vy0: prev.vy1, vz0: prev.vz1,
            vx1: vx, vy1: vy, vz1: vz,
            t0: prev.t1, t1: timestamp
        });
    }

    /**
     * Calculates smooth Hermite interpolated position at exact client render timestamp.
     */
    getExtrapolatedPosition(id, renderTime) {
        const s = this.states.get(id);
        if (!s) return null;

        const dtPacket = (s.t1 - s.t0) / 1000.0;
        
        // Fallback to velocity extrapolation if single packet state
        if (dtPacket <= 1e-4) {
            const dt = Math.max(0, (renderTime - s.t1) / 1000.0);
            return { x: s.x1 + s.vx1 * dt, y: s.y1 + s.vy1 * dt, z: s.z1 + s.vz1 * dt };
        }

        // Hermite Cubic Basis Functions (t in [0, 1])
        const t = Math.min(1.0, Math.max(0.0, (renderTime - s.t0) / (s.t1 - s.t0)));
        const t2 = t * t;
        const t3 = t2 * t;

        const h00 = 2 * t3 - 3 * t2 + 1;
        const h10 = t3 - 2 * t2 + t;
        const h01 = -2 * t3 + 3 * t2;
        const h11 = t3 - t2;

        return {
            x: h00 * s.x0 + h10 * dtPacket * s.vx0 + h01 * s.x1 + h11 * dtPacket * s.vx1,
            y: h00 * s.y0 + h10 * dtPacket * s.vy0 + h01 * s.y1 + h11 * dtPacket * s.vy1,
            z: h00 * s.z0 + h10 * dtPacket * s.vz0 + h01 * s.z1 + h11 * dtPacket * s.vz1
        };
    }
}

if (typeof module !== 'undefined') {
    module.exports = { VelocityExtrapolator };
}
