import numpy as np

class DynamicThreatMatrix:
    """
    Computes real-time threat scores (0.0 to 100.0) for radar targets based on distance,
    velocity vectors, and asset orientation relative to protected zones.
    """
    @staticmethod
    def calculate_threat_score(target_pos: list, target_vel: list, protected_asset_pos: list) -> float:
        pos_diff = np.array(protected_asset_pos) - np.array(target_pos)
        distance = np.linalg_norm(pos_diff)
        
        if distance == 0:
            return 100.0
            
        # Closing velocity calculation (Dot product of velocity and relative position vector)
        vel_vector = np.array(target_vel)
        closing_speed = np.dot(vel_vector, pos_diff / distance)
        
        # Threat Formula: Inverse square distance weighted by closing velocity
        proximity_factor = max(0.0, 100000.0 - distance) / 1000.0
        closing_factor = max(0.0, closing_speed) * 0.5
        
        threat_score = min(100.0, max(0.0, proximity_factor + closing_factor))
        return float(threat_score)

if __name__ == "__main__":
    score = DynamicThreatMatrix.calculate_threat_score([5000.0, 2000.0, 3000.0], [250.0, 0.0, 0.0], [0.0, 0.0, 0.0])
    print(f"[Dynamic Threat Matrix] Real-Time Evaluated Threat Index: {score:.2f}/100.0")
