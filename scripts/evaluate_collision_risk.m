% SkyWatch-20 Collision Probability Matrix
% Purpose: Predict dynamic proximity violations using linear intercept mathematics

clear; clc;

% Aircraft State Vectors: [X, Y, Velocity_X, Velocity_Y]
ac1 = [-5.0, -5.0,  0.15,  0.15];  % Intercept tracking jet A
ac2 = [ 5.0,  5.0, -0.12, -0.12];  % Intercept tracking jet B

% Relative position and velocity vectors
dP = ac1(1:2) - ac2(1:2);
dV = ac1(3:4) - ac2(3:4);

v2 = dot(dV, dV);

if v2 > 0
    % Calculate exact time to Closest Point of Approach (CPA)
    time_to_cpa_seconds = -(dot(dP, dV)) / v2;
    
    if time_to_cpa_seconds < 0
        time_to_cpa_seconds = 0;
    end
    
    % Compute positions at CPA intersection point
    cpa_pos1 = ac1(1:2) + ac1(3:4) * time_to_cpa_seconds;
    cpa_pos2 = ac2(1:2) + ac2(3:4) * time_to_cpa_seconds;
    
    % Final separation delta evaluation
    cpa_distance_nm = norm(cpa_pos1 - cpa_pos2);
    
    fprintf('=== COCKPIT TCAS METRICS ===\n');
    fprintf('Estimated Time to Intercept: %.1f seconds\n', time_to_cpa_seconds);
    fprintf('Predicted Separation Distance: %.2f NM\n', cpa_distance_nm);
else
    fprintf('Aircraft are traveling on perfectly parallel trajectories.\n');
end
