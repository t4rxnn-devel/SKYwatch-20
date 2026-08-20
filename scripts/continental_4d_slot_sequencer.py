"""
Continental 4D Trajectory Predictor & Collaborative Flight Slot Sequencer
Computes 4D waypoint propagation (Lat, Lon, Alt, Time) across multi-sector airspace 
and resolves continent-wide arrival slot congestion via Collaborative Decision Making (CDM).
"""

import numpy as np
import time

class Continental4DSlotSequencer:
    def __init__(self, target_speed_knots=450.0):
        self.target_speed_mps = target_speed_knots * 0.514444
        self.sector_capacity_limit = 20  # Max aircraft per sector window

    def propagate_4d_trajectory(self, waypoints: list, start_time_epoch: float) -> list:
        """
        Propagates 3D waypoints into a time-stamped 4D trajectory (Lat, Lon, Alt, Epoch_Time).
        """
        trajectory_4d = []
        current_time = start_time_epoch

        for i in range(len(waypoints)):
            pt = waypoints[i]
            if i == 0:
                trajectory_4d.append({"lat": pt[0], "lon": pt[1], "alt": pt[2], "eta_epoch": current_time})
                continue
            
            prev_pt = waypoints[i-1]
            # Approximate Haversine/Euclidean distance in meters
            d_lat = (pt[0] - prev_pt[0]) * 111320.0
            d_lon = (pt[1] - prev_pt[1]) * 111320.0 * np.cos(np.radians(pt[0]))
            d_alt = pt[2] - prev_pt[2]
            
            dist_3d = np.sqrt(d_lat**2 + d_lon**2 + d_alt**2)
            flight_time_sec = dist_3d / self.target_speed_mps
            current_time += flight_time_sec
            
            trajectory_4d.append({"lat": pt[0], "lon": pt[1], "alt": pt[2], "eta_epoch": current_time})

        return trajectory_4d

    def resolve_slot_conflict(self, flight_manifest: list, slot_window_sec=120.0) -> list:
        """
        Collaborative Decision Making (CDM) slot negotiator.
        Adjusts Controlled Time of Arrival (CTA) to resolve arrival sector bottlenecks.
        """
        # Sort flights by predicted 4D arrival time
        sorted_manifest = sorted(flight_manifest, key=lambda f: f["trajectory_4d"][-1]["eta_epoch"])
        adjusted_manifest = []
        
        last_slot_time = 0.0
        for flight in sorted_manifest:
            predicted_eta = flight["trajectory_4d"][-1]["eta_epoch"]
            
            if predicted_eta < last_slot_time + slot_window_sec:
                # Slot conflict detected: Assign Controlled Time of Arrival (CTA) delay
                controlled_eta = last_slot_time + slot_window_sec
                flight["assigned_cta"] = controlled_eta
                flight["delay_seconds"] = controlled_eta - predicted_eta
                last_slot_time = controlled_eta
            else:
                flight["assigned_cta"] = predicted_eta
                flight["delay_seconds"] = 0.0
                last_slot_time = predicted_eta

            adjusted_manifest.append(flight)

        return adjusted_manifest

if __name__ == "__main__":
    sequencer = Continental4DSlotSequencer(target_speed_knots=460.0)
    
    # Flight A & Flight B heading to same destination sector
    route1 = [[34.05, -118.24, 10000], [36.16, -115.13, 11000], [40.71, -74.00, 1000]]
    route2 = [[33.94, -118.40, 10000], [36.08, -115.17, 11000], [40.71, -74.00, 1000]]
    
    t_now = time.time()
    traj1 = sequencer.propagate_4d_trajectory(route1, t_now)
    traj2 = sequencer.propagate_4d_trajectory(route2, t_now + 30.0) # Arrives 30s later
    
    flights = [
        {"flight_id": "UAL101", "trajectory_4d": traj1},
        {"flight_id": "AAL204", "trajectory_4d": traj2}
    ]
    
    scheduled_flights = sequencer.resolve_slot_conflict(flights, slot_window_sec=120.0)
    for f in scheduled_flights:
        print(f"[4D Slot Manager] Flight {f['flight_id']} | Assigned CTA: {f['assigned_cta']:.2f} | Delay: {f['delay_seconds']:.1f}s")
