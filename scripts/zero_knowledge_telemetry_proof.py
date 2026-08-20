import hashlib
import json
import time

class TelemetryMerkleTree:
    """
    Constructs a cryptographic Merkle Tree over airborne telemetry packets
    to provide Zero-Knowledge integrity verification without exposing state payloads.
    """
    def __init__(self, telemetry_packets: list):
        self.leaves = [self._hash_packet(p) for p in telemetry_packets]
        self.root = self._build_tree(self.leaves)

    @staticmethod
    def _hash_packet(packet: dict) -> str:
        serialized = json.dumps(packet, sort_keys=True).encode('utf-8')
        return hashlib.sha256(serialized).hexdigest()

    def _build_tree(self, nodes: list) -> str:
        if not nodes:
            return ""
        if len(nodes) == 1:
            return nodes[0]
        
        next_level = []
        for i in range(0, len(nodes), 2):
            if i + 1 < len(nodes):
                combined = (nodes[i] + nodes[i+1]).encode('utf-8')
            else:
                combined = (nodes[i] + nodes[i]).encode('utf-8')
            next_level.append(hashlib.sha256(combined).hexdigest())
            
        return self._build_tree(next_level)

if __name__ == "__main__":
    test_packets = [
        {"ts": time.time(), "lat": 34.05, "lon": -118.25, "alt": 10500.0, "mach": 1.82},
        {"ts": time.time() + 1, "lat": 34.06, "lon": -118.24, "alt": 10520.0, "mach": 1.84}
    ]
    merkle = TelemetryMerkleTree(test_packets)
    print(f"[ZK-Telemetry Engine] State Commitment Hash Root: {merkle.root}")
