function g_ecef = gravity_j2_perturbation(pos_ecef)
    % GRAVITY_J2_PERTURBATION Computes J2-perturbed gravitational acceleration.
    % Input: pos_ecef = [x, y, z] in meters
    % Output: g_ecef = [gx, gy, gz] in m/s^2
    
    GM = 3.986004418e14; % Earth gravitational constant (m^3/s^2)
    a  = 6378137.0;      % WGS-84 semi-major axis (m)
    J2 = 1.08263e-3;     % J2 zonal harmonic factor

    x = pos_ecef(1); y = pos_ecef(2); z = pos_ecef(3);
    r2 = x^2 + y^2 + z^2;
    r = sqrt(r2);

    if r < 1.0
        g_ecef = [0.0, 0.0, 0.0];
        return;
    end

    z2_r2 = (z / r)^2;
    factor = (3/2) * J2 * (a / r)^2;

    % Acceleration components
    gx = - (GM * x / r^3) * (1 - factor * (5 * z2_r2 - 1));
    gy = - (GM * y / r^3) * (1 - factor * (5 * z2_r2 - 1));
    gz = - (GM * z / r^3) * (1 - factor * (5 * z2_r2 - 3));

    g_ecef = [gx, gy, gz];
end
