import numpy as np

class MultiDomainMeshCoordinator:
    """
    Unifies Land, Air, and Undersea autonomous platforms into a single 3D mesh matrix.
    """
    DOMAIN_TYPES = {"LAND_UGV": 1, "AIR_UAV": 2, "UNDERSEA_UUV": 3}

    @classmethod
    def normalize_coordinate_frame(cls, domain: str, pos_raw: np.ndarray) -> np.ndarray:
        # Converts ocean depth (negative Z) and land terrain offsets into unified Cartesian space
        if domain == "UNDERSEA_UUV":
            return np.array([pos_raw[0], pos_raw[1], -abs(pos_raw[2])])
        elif domain == "LAND_UGV":
            return np.array([pos_raw[0], pos_raw[1], max(0.0, pos_raw[2])])
        return pos_raw # AIR_UAV (Standard MSL Altitude)

if __name__ == "__main__":
    uuv_pos = MultiDomainMeshCoordinator.normalize_coordinate_frame("UNDERSEA_UUV", np.array([100.0, 200.0, 50.0]))
    print(f"[Multi-Domain Engine] Undersea Asset Unified Mesh Coordinate: {uuv_pos}")
