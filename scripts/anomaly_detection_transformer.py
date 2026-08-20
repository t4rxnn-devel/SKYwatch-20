import numpy as np

def softmax(x: np.ndarray) -> np.ndarray:
    e_x = np.exp(x - np.max(x, axis=-1, keepdims=True))
    return e_x / np.sum(e_x, axis=-1, keepdims=True)

class MiniSelfAttention:
    """
    Scaled Dot-Product Self-Attention module for dynamic target trajectory sequences.
    """
    def __init__(self, embed_dim: int = 8):
        self.embed_dim = embed_dim
        # Random initial weight matrices for Query, Key, Value
        np.random.seed(42)
        self.W_q = np.random.randn(embed_dim, embed_dim) * 0.1
        self.W_k = np.random.randn(embed_dim, embed_dim) * 0.1
        self.W_v = np.random.randn(embed_dim, embed_dim) * 0.1

    def forward(self, trajectory_sequence: np.ndarray) -> np.ndarray:
        # trajectory_sequence shape: (seq_len, embed_dim)
        Q = np.dot(trajectory_sequence, self.W_q)
        K = np.dot(trajectory_sequence, self.W_k)
        V = np.dot(trajectory_sequence, self.W_v)

        d_k = self.embed_dim
        scores = np.dot(Q, K.T) / np.sqrt(d_k)
        attention_weights = softmax(scores)
        
        output = np.dot(attention_weights, V)
        return output, attention_weights

if __name__ == "__main__":
    # Sequence of 5 state vectors: [x, y, z, vx, vy, vz, ax, ay]
    traj_data = np.random.randn(5, 8)
    transformer = MiniSelfAttention(embed_dim=8)
    attended_feats, weights = transformer.forward(traj_data)
    print(f"[Attention Engine] Target Trajectory Self-Attention Matrix Shape: {weights.shape}")
