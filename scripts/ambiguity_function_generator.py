import numpy as np

def compute_woodward_ambiguity(signal: np.ndarray, fs: float) -> np.ndarray:
    """
    Calculates the 2D Woodward Ambiguity Function |Chi(tau, fd)| over Delay and Doppler.
    """
    N = len(signal)
    delays = np.arange(-N + 1, N)
    doppler_bins = np.fft.fftfreq(N, d=1/fs)
    
    ambiguity = np.zeros((len(doppler_bins), len(delays)), dtype=np.complex128)
    
    # Continuous correlation across delay and phase shift
    for i, tau in enumerate(delays):
        if tau >= 0:
            s_shifted = np.pad(signal, (0, tau))[tau:]
            s_orig = signal
        else:
            s_shifted = signal[:tau]
            s_orig = np.pad(signal, (-tau, 0))[:-tau]
            
        for j, fd in enumerate(doppler_bins):
            phase_vector = np.exp(1j * 2 * np.pi * fd * np.arange(len(s_orig)) / fs)
            ambiguity[j, i] = np.sum(s_orig * np.conj(s_shifted) * phase_vector)
            
    return np.abs(ambiguity)

if __name__ == "__main__":
    fs = 1e6 # 1 MHz sampling
    t = np.linspace(0, 1e-3, int(fs * 1e-3))
    # Linear Frequency Modulation (LFM Chirp)
    chirp_signal = np.exp(1j * np.pi * (10e6 / 1e-3) * t**2)
    
    af = compute_woodward_ambiguity(chirp_signal, fs)
    print(f"[DSP Module] Ambiguity Matrix Computed: Shape {af.shape}, Peak Power {np.max(af):.2f}")
