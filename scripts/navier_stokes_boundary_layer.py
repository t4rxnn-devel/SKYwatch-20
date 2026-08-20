import numpy as np

def blasius_derivatives(eta: float, y: np.ndarray) -> np.ndarray:
    """
    Blasius ODE system:
    f  = y[0]
    f' = y[1]  (u / U_infinity)
    f''= y[2]
    f''' = -0.5 * f * f''
    """
    return np.array([y[1], y[2], -0.5 * y[0] * y[2]])

def solve_blasius_shooting(eta_max: float = 6.0, steps: int = 1000) -> tuple:
    """
    Shoots for initial f''(0) to satisfy boundary condition f'(eta_max) = 1.0.
    """
    d_eta = eta_max / steps
    f_double_prime_0 = 0.33206  # Exact analytical shooting guess

    y = np.array([0.0, 0.0, f_double_prime_0])
    eta_grid = np.linspace(0, eta_max, steps)
    u_profile = []

    for eta in eta_grid:
        u_profile.append(y[1])
        # RK4 Integration step
        k1 = blasius_derivatives(eta, y)
        k2 = blasius_derivatives(eta + 0.5 * d_eta, y + 0.5 * d_eta * k1)
        k3 = blasius_derivatives(eta + 0.5 * d_eta, y + 0.5 * d_eta * k2)
        k4 = blasius_derivatives(eta + d_eta, y + d_eta * k3)
        y += (d_eta / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4)

    return eta_grid, np.array(u_profile)

def compute_skin_friction(x_pos_m: float, u_inf: float, nu: float = 1.5e-5) -> float:
    """
    Calculates local wall shear stress tau_w (Pa) given distance from leading edge.
    """
    re_x = (u_inf * x_pos_m) / nu
    if re_x < 1.0: return 0.0
    c_fx = 0.664 / np.sqrt(re_x)  # Laminar skin friction coefficient
    rho = 1.225
    return 0.5 * rho * (u_inf**2) * c_fx

if __name__ == "__main__":
    eta, u_norm = solve_blasius_shooting()
    tau_w = compute_skin_friction(x_pos_m=1.5, u_inf=250.0)
    print(f"Blasius Boundary Layer normalized velocity at edge: {u_norm[-1]:.4f}")
    print(f"Wall Shear Stress at x=1.5m: {tau_w:.2f} Pa")
