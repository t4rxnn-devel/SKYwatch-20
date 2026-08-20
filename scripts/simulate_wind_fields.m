% SkyWatch-20 Vector Field Storm Modeler
% Purpose: Simulate the localized aerodynamic wind shear force directions

clear; clc;

% Define a grid array space spanning from -20 to +20 Nautical Miles
[X, Y] = meshgrid(-20:4:20, -20:4:20);

% Initialize zero wind velocity arrays across the grid matrix
U = zeros(size(X));
V = zeros(size(Y));

% Configure a mock storm center position vector coordinate 
storm_center = [8.0, 6.0];
storm_radius = 8.0;

% Compute localized shear gradient push angles inside the storm radius
for i = 1:numel(X)
    dist = sqrt((X(i) - storm_center(1))^2 + (Y(i) - storm_center(2))^2);
    if dist <= storm_radius
        % Rotational cyclonic force simulation matching Doppler parameters
        U(i) = -(Y(i) - storm_center(2)) * 0.2;
        V(i) = (X(i) - storm_center(1)) * 0.2;
    end
end

disp('=== WIND INTERACTION MATRIX CONFIGURED ===');
disp('Aerodynamic shear vectors successfully evaluated inside grid spaces.');
