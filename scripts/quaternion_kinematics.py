import numpy as np

class QuaternionKinematics:
    """
    Computes 3D singularity-free flight orientation and attitude propagation.
    Eliminates Gimbal Lock using quaternion differential equations: dq/dt = 0.5 * q (x) omega
    """
    def __init__(self, w: float = 1.0, x: float = 0.0, y: float = 0.0, z: float = 0.0):
        self.q = np.array([w, x, y, z], dtype=np.float64)
        self.normalize()

    def normalize(self):
        norm = np.linalg.norm(self.q)
        if norm > 1e-12:
            self.q /= norm

    def propagate_angular_velocity(self, omega_rad_s: np.ndarray, dt: float):
        """
        Propagates target attitude given body-frame angular velocity vector [wx, wy, wz].
        """
        wx, wy, wz = omega_rad_s
        # Quaternion rate matrix Omega
        Omega = np.array([
            [0.0, -wx,  -wy,  -wz],
            [wx,   0.0,   wz,  -wy],
            [wy,  -wz,   0.0,   wx],
            [wz,   wy,  -wx,   0.0]
        ])
        
        dq_dt = 0.5 * np.dot(Omega, self.q)
        self.q += dq_dt * dt
        self.normalize()

    def to_rotation_matrix(self) -> np.ndarray:
        """
        Converts unit quaternion to a 3x3 direction cosine matrix (DCM).
        """
        w, x, y, z = self.q
        return np.array([
            [1 - 2*(y**2 + z**2),   2*(x*y - z*w),       2*(x*z + y*w)],
            [2*(x*y + z*w),       1 - 2*(x**2 + z**2),   2*(y*z - x*w)],
            [2*(x*z - y*w),       2*(y*z + x*w),       1 - 2*(x**2 + y**2)]
        ])

if __name__ == "__main__":
    attitude = QuaternionKinematics()
    angular_rates = np.array([0.05, 0.2, -0.01])  # Pitch up & roll rate
    dt = 0.01
    
    for _ in range(100):
        attitude.propagate_angular_velocity(angular_rates, dt)
        
    print(f"Propagated Unit Quaternion [w, x, y, z]: {np.round(attitude.q, 4)}")
    print(f"Direction Cosine Matrix:\n{np.round(attitude.to_rotation_matrix(), 3)}")
