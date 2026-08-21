"""
Hyperdimensional Computing (HDC) Trajectory Classifier
Encodes spatial-temporal radar trajectories into 10,000D bipolar hypervectors
and classifies flight maneuvers via vector symbolic inner product operations.
"""

import numpy as np

class HyperdimensionalTrajectoryClassifier:
    def __init__(self, dimension=10000, num_levels=100):
        self.D = dimension
        self.num_levels = num_levels
        np.random.seed(42)
        
        # Base hypervectors for spatial coordinates X, Y, Z
        self.base_X = np.random.choice([-1, 1], size=self.D)
        self.base_Y = np.random.choice([-1, 1], size=self.D)
        self.base_Z = np.random.choice([-1, 1], size=self.D)
        
        # Continuous value level hypervectors
        self.level_hvs = self._generate_level_hypervectors()
        
        # Class Memory Prototype Store
        self.prototypes = {}

    def _generate_level_hypervectors(self) -> np.ndarray:
        """Generates continuous level hypervectors with linear correlation."""
        hvs = np.zeros((self.num_levels, self.D), dtype=np.int8)
        base = np.random.choice([-1, 1], size=self.D)
        hvs[0] = base
        
        flip_per_level = self.D // (2 * self.num_levels)
        curr = base.copy()
        
        for i in range(1, self.num_levels):
            flip_indices = np.random.choice(self.D, size=flip_per_level, replace=False)
            curr[flip_indices] *= -1
            hvs[i] = curr
        return hvs

    def _value_to_level(self, val: float, val_min=-10000.0, val_max=10000.0) -> int:
        norm = (val - val_min) / (val_max - val_min)
        idx = int(norm * (self.num_levels - 1))
        return max(0, min(self.num_levels - 1, idx))

    def encode_point(self, x: float, y: float, z: float) -> np.ndarray:
        """
        Binds coordinate attributes using element-wise multiplication (XOR in binary HDC).
        """
        hv_x = self.base_X * self.level_hvs[self._value_to_level(x)]
        hv_y = self.base_Y * self.level_hvs[self._value_to_level(y)]
        hv_z = self.base_Z * self.level_hvs[self._value_to_level(z)]
        
        # Bundling (Superposition) via element-wise addition and sign thresholding
        point_hv = np.sign(hv_x + hv_y + hv_z)
        point_hv[point_hv == 0] = 1
        return point_hv

    def encode_trajectory(self, trajectory: list) -> np.ndarray:
        """
        Encodes a sequential trajectory using permutation-based time binding.
        """
        traj_hv = np.zeros(self.D)
        for t, pt in enumerate(trajectory):
            pt_hv = self.encode_point(pt[0], pt[1], pt[2])
            # Time permutation via vector circular shift
            shifted_hv = np.roll(pt_hv, t)
            traj_hv += shifted_hv
            
        final_hv = np.sign(traj_hv)
        final_hv[final_hv == 0] = 1
        return final_hv

    def train_class(self, label: str, trajectories: list):
        """Train class prototype by bundling training hypervectors."""
        class_hv = np.zeros(self.D)
        for traj in trajectories:
            class_hv += self.encode_trajectory(traj)
        
        proto = np.sign(class_hv)
        proto[proto == 0] = 1
        self.prototypes[label] = proto

    def classify(self, trajectory: list) -> tuple:
        """Classifies an unknown trajectory via Cosine Similarity in hyperdimensional space."""
        query_hv = self.encode_trajectory(trajectory)
        best_label = "UNKNOWN"
        max_sim = -1.0
        
        for label, proto in self.prototypes.items():
            sim = np.dot(query_hv, proto) / self.D  # Cosine similarity for bipolar vectors
            if sim > max_sim:
                max_sim = sim
                best_label = label
                
        return best_label, float(max_sim)

if __name__ == "__main__":
    hdc = HyperdimensionalTrajectoryClassifier()
    
    # Generate synthetic training patterns
    straight_flight = [[i*100.0, 0.0, 5000.0] for i in range(20)]
    evasive_turn = [[i*100.0, (i**2)*10.0, 5000.0 - i*50.0] for i in range(20)]
    
    hdc.train_class("COMMERCIAL_PATROL", [straight_flight])
    hdc.train_class("EVASIVE_MANEUVER", [evasive_turn])
    
    # Test query trajectory
    test_query = [[i*105.0, (i**2)*9.5, 4990.0 - i*48.0] for i in range(20)]
    match, similarity = hdc.classify(test_query)
    
    print(f"[HD Neural Classifier] Recognized Profile: {match} | Hyperdimensional Similarity: {similarity:.4f}")
