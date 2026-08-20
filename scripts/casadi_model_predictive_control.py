import casadi as ca
import numpy as np

def build_nmpc_solver(horizon=10, dt=0.1):
    """
    Nonlinear Model Predictive Control (NMPC) for high-maneuver intercept trajectories.
    """
    # States: [x, y, vx, vy], Controls: [ax, ay]
    x = ca.MX.sym('x', 4)
    u = ca.MX.sym('u', 2)

    # State derivative system ODE
    xdot = ca.vertcat(x[2], x[3], u[0], u[1])
    f = ca.Function('f', [x, u], [xdot])

    # Optimization Variables
    X = ca.MX.sym('X', 4, horizon + 1)
    U = ca.MX.sym('U', 2, horizon)
    P = ca.MX.sym('P', 8) # [X_init (4), X_target (4)]

    cost = 0
    g = [X[:, 0] - P[:4]] # Initial condition constraint

    for k in range(horizon):
        # Kinematic integration step
        st_next = X[:, k] + dt * f(X[:, k], U[:, k])
        g.append(X[:, k+1] - st_next)

        # Objective: Minimize distance to target + Control Effort
        target_error = X[:2, k+1] - P[4:6]
        cost += ca.mtimes(target_error.T, target_error) + 0.01 * ca.mtimes(U[:, k].T, U[:, k])

    nlp = {'f': cost, 'x': ca.vertcat(ca.reshape(X, -1, 1), ca.reshape(U, -1, 1)), 'g': ca.vertcat(*g), 'p': P}
    solver = ca.nlpsol('solver', 'ipopt', nlp, {'ipopt.print_level': 0, 'print_time': 0})
    return solver

if __name__ == "__main__":
    solver = build_nmpc_solver()
    print("[NMPC Module] CasADi Symbolic MPC Trajectory Solver Initialized.")
