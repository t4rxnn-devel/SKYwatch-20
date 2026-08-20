% SkyWatch-20 Trajectory Prediction & Modeling Suite
% Purpose: Matrix simulation of 3D target updates across weather vectors

clear; clc;

% Define Initial Target Fleet Matrix [X, Y, Velocity_X, Velocity_Y]
fleet = [
    -15.0, -15.0,  0.10,  0.10;  % UAL104
     15.0,  15.0, -0.09, -0.09;  % AAL892
    -18.0,  10.0,  0.12, -0.05   % DAL440
];

callsigns = {'UAL104', 'AAL892', 'DAL440'};
time_steps = 100;
dt = 0.5;

figure('Name', 'SkyWatch-20 Aerospace Tracking Simulation', 'Color', [0.01, 0.05, 0.02]);
hold on;
grid on;

% Aesthetic setup matching our dark-green military look
set(gca, 'Color', [0.01, 0.06, 0.02], 'GridColor', [0.0, 0.4, 0.1], 'XColor', [0.0, 0.8, 0.3], 'YColor', [0.0, 0.8, 0.3]);
title('=== SKYWATCH-20 TACTICAL PROJECTION BOARD ===', 'Color', [0.0, 1.0, 0.5]);
xlabel('X Coordinates (Nautical Miles)');
ylabel('Y Coordinates (Nautical Miles)');

% Plot paths over time frames
for t = 1:time_steps
    for p = 1:size(fleet, 1)
        % Update positions with linear velocity physics equations
        fleet(p, 1) = fleet(p, 1) + fleet(p, 3) * dt;
        fleet(p, 2) = fleet(p, 2) + fleet(p, 4) * dt;
        
        % Render flight markers dynamically
        plot(fleet(p, 1), fleet(p, 2), 'g.', 'MarkerSize', 8);
    end
    pause(0.05); % Simulates real-time sweep intervals
end

hold off;
