import numpy as np
from scipy.spatial.transform import Rotation as R

def calculate_conjunction_probability(pos1: np.ndarray, cov1: np.ndarray, 
                                      pos2: np.ndarray, cov2: np.ndarray, 
                                      hard_body_radius: float) -> float:
    """
    Computes 2D Space Debris Conjunction Probability (Foster 1992 Algorithm)
    """
    # Relative position vector in encounter frame
    r_rel = pos2 - pos1
    distance = np.linalg.norm(r_rel)
    
    # Combined error covariance matrix
    cov_combined = cov1 + cov2
    
    # Project covariance matrix onto the collision plane normal to relative velocity
    v_rel = np.array([0.0, 7800.0, 10.0]) # Example relative velocity vector (m/s)
    ez = v_rel / np.linalg.norm(v_rel)
    ex = np.cross(ez, r_rel) / np.linalg.norm(np.cross(ez, r_rel))
    ey = np.cross(ez, ex)
    
    M = np.vstack([ex, ey, ez])
    cov_plane = np.dot(M, np.dot(cov_combined, M.T))[:2, :2]
    
    # 2D Probability Density Function integration over Hard Body Radius
    det_cov = np.linalg.det(cov_plane)
    inv_cov = np.linalg.inv(cov_plane)
    
    r_proj = np.dot(M[:2, :], r_rel)
    b_distance = np.dot(r_proj.T, np.dot(inv_cov, r_proj))
    
    p_c = (hard_body_radius**2 / (2 * np.sqrt(det_cov))) * np.exp(-0.5 * b_distance)
    return float(p_c)

if __name__ == "__main__":
    sat_pos = np.array([7000e3, 0.0, 0.0])
    sat_cov = np.eye(3) * 25.0 # 5m stddev
    debris_pos = np.array([7000e3 + 12.0, 5.0, 2.0])
    debris_cov = np.eye(3) * 100.0 # 10m stddev
    
    prob = calculate_conjunction_probability(sat_pos, sat_cov, debris_pos, debris_cov, hard_body_radius=2.5)
    print(f"[SSA Module] Orbital Conjunction Risk Calculated: Pc = {prob:.8e}")
