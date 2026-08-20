# SkyWatch-20 Automated Safety-Critical Rule Validator
# Compliant with RTCA DO-178C / EUROCONTROL software integrity audits

import math
import sys

# Standard separation minimum bounds
MIN_HORIZONTAL_SEPARATION_NM = 5.0
MIN_VERTICAL_SEPARATION_FT = 1000.0

def audit_airspace_safety(aircraft_fleet):
    """
    Scans a fleet matrix to verify strict horizontal and vertical separation limits.
    """
    fault_detected = False
    num_planes = len(aircraft_fleet)
    
    if num_planes < 2:
        return True

    for i in range(num_planes):
        for j in range(i + 1, num_planes):
            ac1 = aircraft_fleet[i]
            ac2 = aircraft_fleet[j]
            
            # 2D Horizontal distance check
            dx = ac1['x'] - ac2['x']
            dy = ac1['y'] - ac2['y']
            horizontal_dist = math.sqrt(dx**2 + dy**2)
            
            # Vertical gap check
            vertical_dist = abs(ac1['alt'] - ac2['alt'])
            
            # If BOTH boundaries are violated simultaneously, a critical loss of separation occurs
            if horizontal_dist < MIN_HORIZONTAL_SEPARATION_NM and vertical_dist < MIN_VERTICAL_SEPARATION_FT:
                print(f"❌ [DO-178C VIOLATION]: Separation failure between {ac1['id']} and {ac2['id']}!")
                print(f"   Horizontal: {horizontal_dist:.2f} NM (Min: {MIN_HORIZONTAL_SEPARATION_NM} NM)")
                print(f"   Vertical:   {vertical_dist:.1f} FT (Min: {MIN_VERTICAL_SEPARATION_FT} FT)")
                fault_detected = True
                
    return not fault_detected

if __name__ == "__main__":
    # Test suite mock snapshot matching our core simulation
    mock_fleet = [
        {"id": "UAL104", "x": -15.0, "y": -15.0, "alt": 5000.0},
        {"id": "AAL892", "x": -12.4, "y": -14.1, "alt": 5400.0},  # Intentionally breaking separation limits
        {"id": "DAL440", "x": 10.0, "y": 18.0, "alt": 8000.0}
    ]
    
    print("=== STARTING RUNTIME SEPARATION SAFETY AUDIT ===")
    is_safe = audit_airspace_safety(mock_fleet)
    if is_safe:
        print("✅ Airspace safety criteria fully satisfied.")
        sys.exit(0)
    else:
        sys.exit(1)
