import numpy as np

def capon_mvdr_beamformer(array_geometry: np.ndarray, signal_angle_deg: float, 
                           jammer_angles_deg: list, snr_db: float, jnr_db: float) -> np.ndarray:
    """
    Computes optimal complex weights for phased array antenna to null out EW jammers.
    """
    num_elements = len(array_geometry)
    wavelength = 0.19029 # GPS L1 Wavelength (m)
    
    def steering_vector(angle_deg):
        rad = np.radians(angle_deg)
        k = 2 * np.pi / wavelength
        phase_shifts = k * array_geometry * np.sin(rad)
        return np.exp(1j * phase_shifts)

    # Desired Satellite Signal Steering Vector
    s_desired = steering_vector(signal_angle_deg)
    
    # Generate Noise + Jammer Covariance Matrix (R_nn)
    R_nn = np.eye(num_elements, dtype=np.complex128) * (10**(-snr_db / 10.0))
    for j_angle in jammer_angles_deg:
        s_jammer = steering_vector(j_angle)
        j_power = 10**(jnr_db / 10.0)
        R_nn += j_power * np.outer(s_jammer, np.conj(s_jammer))

    # Capon MVDR Optimal Weight Vector Formulation: w = (R_nn^-1 * s) / (s^H * R_nn^-1 * s)
    R_inv = np.linalg.inv(R_nn)
    numerator = np.dot(R_inv, s_desired)
    denominator = np.dot(np.conj(s_desired).T, numerator)
    
    weights = numerator / denominator
    return weights

if __name__ == "__main__":
    # 8-Element Uniform Linear Antenna Array
    elements = np.linspace(0, 7 * 0.095, 8) 
    w_opt = capon_mvdr_beamformer(elements, signal_angle_deg=15.0, jammer_angles_deg=[-45.0, 60.0], snr_db=10, jnr_db=40)
    print(f"[EW Defense Engine] Adaptive Array Beamforming Weights Computed (Nulls placed at -45° and +60°).")
