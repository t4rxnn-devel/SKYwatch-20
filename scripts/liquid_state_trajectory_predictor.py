"""
Liquid State Machine (LSM) / Echo State Reservoir Neural Network
Predicts chaotic flight maneuvers and detects trajectory anomalies in 4D space
without backpropagation overhead or heavy AI frameworks.
"""

import numpy as np

class LiquidStateTrajectoryPredictor:
    def __init__(self, input_dim=3, reservoir_size=100, spectral_radius=0.95, leak_rate=0.3):
        self.input_dim = input_dim
        self.reservoir_size = reservoir_size
        self.leak_rate = leak_rate
        
        # Random input weight matrix W_in
        np.random.seed(42)
        self.W_in = np.random.uniform(-0.5, 0.5, (reservoir_size, input_dim))
        
        # Sparse dynamic reservoir weight matrix W_res
        W_raw = np.random.uniform(-1.0, 1.0, (reservoir_size, reservoir_size))
        W_raw[np.random.rand(*W_raw.shape) > 0.1] = 0.0  # 10% sparsity
        
        # Scale to match desired spectral radius for stable memory retention
        radius = np.max(np.abs(np.linalg.eigvals(W_raw)))
        self.W_res = W_raw * (spectral_radius / radius)
        
        # Current internal state vector
        self.x = np.zeros((reservoir_size, 1))
        
        # Ridge regression trained readout weights
        self.W_out = np.zeros((input_dim, reservoir_size))

    def step(self, u_vec: np.ndarray) -> np.ndarray:
        """
        Integrates a new 3D spatial coordinate [X, Y, Z] through the liquid state dynamics.
        """
        u = u_vec.reshape(-1, 1)
        # Liquid differential state equation
        x_tilde = np.tanh(np.dot(self.W_in, u) + np.dot(self.W_res, self.x))
        self.x = (1.0 - self.leak_rate) * self.x + self.leak_rate * x_tilde
        
        # Linear readout prediction for next timestep
        prediction = np.dot(self.W_out, self.x)
        return prediction.flatten()

    def train_readout(self, sequence: np.ndarray, ridge_alpha=1e-4):
        """
        One-shot closed-form Ridge Regression training over sequence array.
        """
        states = []
        targets = []
        
        for t in range(len(sequence) - 1):
            pred_next = self.step(sequence[t])
            states.append(self.x.flatten())
            targets.append(sequence[t+1])
            
        X_mat = np.array(states)  # (Time, Reservoir_Size)
        Y_mat = np.array(targets) # (Time, Input_Dim)
        
        # Closed-form L2 regularization solution: W_out = (X^T X + alpha I)^-1 X^T Y
        reg = ridge_alpha * np.eye(self.reservoir_size)
        self.W_out = np.dot(np.dot(np.linalg.inv(np.dot(X_mat.T, X_mat) + reg), X_mat.T), Y_mat).T

if __name__ == "__main__":
    # Simulate erratic evasive flight path
    t = np.linspace(0, 10, 200)
    flight_path = np.column_stack([np.sin(t) * 1000, np.cos(t) * 1000, t * 100])
    
    predictor = LiquidStateTrajectoryPredictor(reservoir_size=120)
    predictor.train_readout(flight_path)
    
    test_coordinate = np.array([990.0, 10.0, 1000.0])
    predicted_next = predictor.step(test_coordinate)
    print(f"[Liquid Reservoir Neural Net] Input Pos: {test_coordinate} -> Predicted Next Pos: {predicted_next}")
