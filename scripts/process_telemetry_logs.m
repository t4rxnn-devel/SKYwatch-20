% SkyWatch-20 Telemetry Log Processing Core
% Purpose: Analyze aircraft trajectory histories and output performance telemetry

clear; clc;

% Mock Log Matrix: [Timestamp(s), Position_X(NM), Position_Y(NM), Altitude(FT)]
flight_log = [
    0.0, -15.0, -15.0, 5000;
    2.0, -14.8, -14.6, 5050;
    4.0, -14.5, -14.1, 5100;
    6.0, -14.1, -13.5, 5120;
    8.0, -13.6, -12.8, 5130
];

time = flight_log(:, 1);
x_pos = flight_log(:, 2);
y_pos = flight_log(:, 3);
alt = flight_log(:, 4);

% Compute Euclidean step displacements between timeline nodes
total_distance_nm = 0;
for i = 1:(length(flight_log) - 1)
    dx = x_pos(i+1) - x_pos(i);
    dy = y_pos(i+1) - y_pos(i);
    total_distance_nm = total_distance_nm + sqrt(dx^2 + dy^2);
end

fprintf('=== TELEMETRY SUMMARY PARSE ===\n');
fprintf('Total Airspace Displacement: %.3f NM\n', total_distance_nm);
fprintf('Mean Operational Altitude:    %.1f FT\n', mean(alt));
