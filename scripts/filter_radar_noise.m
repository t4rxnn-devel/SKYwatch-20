% SkyWatch-20 Signal Smoothing Matrix
% Purpose: Filter static interference and atmospheric noise from radar feeds

clear; clc;

% Simulate true smooth flight track coordinates over 10 ticks
true_track = 1:10; 

% Inject random Gaussian atmospheric measurement noise variance
noise = 0.4 * randn(1, 10);
raw_sensor_feed = true_track + noise;

% Apply a 3-point running moving average smoothing filter window
filtered_track = zeros(1, 10);
for i = 1:10
    if i == 1
        filtered_track(i) = raw_sensor_feed(i);
    } else if i == 10
        filtered_track(i) = raw_sensor_feed(i);
    else
        filtered_track(i) = (raw_sensor_feed(i-1) + raw_sensor_feed(i) + raw_sensor_feed(i+1)) / 3.0;
    end
end

disp('=== SENSOR FILTER DATA MATCH ===');
disp('Raw Corrupted Input Feed:'); disp(raw_sensor_feed);
disp('Filtered True Output Matrix:'); disp(filtered_track);
