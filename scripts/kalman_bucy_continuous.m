function [x_dot, P_dot] = kalman_bucy_continuous(x_hat, P, z, A, C, Q, R)
    % KALMAN_BUCY_CONTINUOUS Continuous-time optimal filter derivative step
    % Inputs:
    %   x_hat : Current state estimate vector [n x 1]
    %   P     : Error covariance matrix [n x n]
    %   z     : Continuous measurement vector [m x 1]
    %   A     : State transition differential matrix
    %   C     : Measurement observation matrix
    %   Q     : Process noise spectral density matrix
    %   R     : Measurement noise spectral density matrix

    % Continuous Kalman Gain: K(t) = P(t) * C^T * R^-1
    K = P * (C') / R;

    % State estimate rate: dx_hat/dt = A * x_hat + K * (z - C * x_hat)
    x_dot = A * x_hat + K * (z - C * x_hat);

    % Matrix Riccati differential equation: dP/dt = A*P + P*A' - P*C'*R^-1*C*P + Q
    P_dot = A * P + P * (A') - (P * (C') / R) * C * P + Q;
end
