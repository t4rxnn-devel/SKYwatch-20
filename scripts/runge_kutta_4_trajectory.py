import numpy as np

def rk4_step(state: np.ndarray, dt: float, deriv_func) -> np.ndarray:
    """
    Executes a single 4th-Order Runge-Kutta step.
    state: np.ndarray of current state variables
    dt: time step size (seconds)
    deriv_func: callable function f(state) -> state_derivative
    """
    k1 = deriv_func(state)
    k2 = deriv_func(state + 0.5 * dt * k1)
    k3 = deriv_func(state + 0.5 * dt * k2)
    k4 = deriv_func(state + dt * k3)
    
    return state + (dt / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4)

if __name__ == "__main__":
    # Example test: 3D point-mass state [x, y, z, vx, vy, vz]
    initial_state = np.array([0.0, 1000.0, 5000.0, 250.0, 0.0, -9.81])
    
    def simple_dynamics(s):
        # ds/dt = [vx, vy, vz, ax, ay, az]
        return np.array([s[3], s[4], s[5], 0.0, 0.0, -9.80665])

    next_state = rk4_step(initial_state, 0.1, simple_dynamics)
    print(f"RK4 Integration Output State: {np.round(next_state, 4)}")
