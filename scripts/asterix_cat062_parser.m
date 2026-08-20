function track_data = asterix_cat062_parser(raw_bytes)
    % ASTERIX_CAT062_PARSER Decodes EUROCONTROL Cat 062 Track Data
    % Input: raw_bytes - uint8 array of ASTERIX payload
    
    if length(raw_bytes) < 8
        error('Invalid ASTERIX Cat 062 frame length.');
    end
    
    % Extract SAC/SIC System Identification (I062/010)
    sac = raw_bytes(1);
    sic = raw_bytes(2);
    
    % Extract Track Number (I062/040)
    track_number = double(raw_bytes(3)) * 256 + double(raw_bytes(4));
    
    % Extract Calculated Track Velocity in Cartesian Coordinates (I062/185)
    % Scale factor: 0.25 m/s per LSB
    vx = double(typecast(raw_bytes(5:6), 'int16')) * 0.25;
    vy = double(typecast(raw_bytes(7:8), 'int16')) * 0.25;
    
    track_data = struct('SAC', sac, 'SIC', sic, ...
                        'TrackNumber', track_number, ...
                        'Vx', vx, 'Vy', vy);
end
