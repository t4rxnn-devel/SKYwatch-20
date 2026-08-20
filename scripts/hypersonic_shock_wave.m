function shock_props = hypersonic_shock_wave(mach1, shock_angle_deg, gamma)
    % HYPERSONIC_SHOCK_WAVE Solves Rankine-Hugoniot jump relations across an oblique shock.
    if nargin < 3, gamma = 1.4; end
    
    beta = deg2rad(shock_angle_deg);
    Mn1 = mach1 * sin(beta); % Normal Mach number component
    
    if Mn1 < 1.0
        error('Normal Mach component must be supersonic for shock formation.');
    end
    
    % Pressure and Temperature ratios across shock
    p_ratio = 1 + (2 * gamma / (gamma + 1)) * (Mn1^2 - 1);
    rho_ratio = ((gamma + 1) * Mn1^2) / ((gamma - 1) * Mn1^2 + 2);
    T_ratio = p_ratio / rho_ratio;
    
    % Post-shock normal Mach number
    Mn2 = sqrt(((gamma - 1) * Mn1^2 + 2) / (2 * gamma * Mn1^2 - (gamma - 1)));
    
    shock_props = struct('PressureRatio', p_ratio, ...
                         'DensityRatio', rho_ratio, ...
                         'TemperatureRatio', T_ratio, ...
                         'PostShockNormalMach', Mn2);
end
