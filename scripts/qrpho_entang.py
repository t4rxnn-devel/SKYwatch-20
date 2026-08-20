import numpy as np

def quantum_illumination_detection_probability(
    n_s: float, n_b: float, kappa: float, M: int, snr_dB_threshold: float
) -> float:
    """
    Computes Quantum Chernoff Bound for Quantum Illumination Radar.
    n_s : Mean photon number per signal mode (n_s << 1)
    n_b : Mean thermal background photon number per mode (n_b >> 1)
    kappa : Atmospheric round-trip transmissivity (0 < kappa <= 1)
    M : Time-bandwidth product (number of integrated independent modes)
    """
    # Quantum Chernoff Bound exponent for Quantum Illumination
    # xi_QI = kappa * n_s / n_b (up to 6dB advantage over classical xi_CI = kappa * n_s / (4 * n_b))
    xi_QI = (kappa * n_s) / n_b
    
    # Error probability bound: P_e <= 0.5 * exp(-M * xi_QI)
    p_error_bound = 0.5 * np.exp(-M * xi_QI)
    p_detection = 1.0 - p_error_bound
    return float(np.clip(p_detection, 0.0, 1.0))

if __name__ == "__main__":
    # Test Parameters: Extremely low signal energy (n_s=0.01) in high thermal jammer noise (n_b=100.0)
    p_det = quantum_illumination_detection_probability(
        n_s=0.01, n_b=100.0, kappa=0.15, M=100000, snr_dB_threshold=3.0
    )
    print(f"[Quantum Physics Engine] Sub-Noise Quantum Target Detection Probability: {p_det:.6f}")
