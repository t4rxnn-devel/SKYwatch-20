import numpy as np

def compute_relativistic_doppler(f0_hz: float, velocity_m_s: float, angle_rad: float) -> float:
    """
    Computes exact relativistic Doppler shift for hyper-velocity target tracking.
    f0_hz        : Transmitted radar carrier frequency (Hz)
    velocity_m_s : Target velocity relative to observer (m/s)
    angle_rad    : Aspect angle between line-of-sight and velocity vector (rad)
    """
    c = 299792458.0  # Speed of light in vacuum (m/s)
    beta = velocity_m_s / c
    gamma = 1.0 / np.sqrt(1.0 - beta**2)
    
    # Longitudinal + Transverse Relativistic Doppler Effect
    f_observed = f0_hz / (gamma * (1.0 - beta * np.cos(angle_rad)))
    return f_observed

if __name__ == "__main__":
    carrier_freq = 10.0e9  # 10 GHz X-band radar
    v_hypersonic = 2500.0   # Mach ~7.3 target velocity
    theta = np.radians(15.0) # 15 degree aspect angle
    
    f_shifted = compute_relativistic_doppler(carrier_freq, v_hypersonic, theta)
    print(f"Relativistic Carrier Frequency Shift: {f_shifted - carrier_freq:.2f} Hz")
