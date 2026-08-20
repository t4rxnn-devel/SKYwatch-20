# SkyWatch-20 Atmospheric Fluid Dynamics and Crosswind Simulator
# Simulates discrete wind vector fields across a multi-cell grid space

import numpy as np

class AtmosphericFluidSim:
    def __init__(self, grid_size=10):
        self.grid_size = grid_size
        # Instantiate localized horizontal (U) and vertical (V) wind speed grids
        self.wind_u = np.zeros((grid_size, grid_size), dtype=float)
        self.wind_v = np.zeros((grid_size, grid_size), dtype=float)
        
    def inject_microburst_force(self, center_x, center_y, intensity):
        """
        Generates a downburst weather force gradient cell radiating outwards.
        """
        for x in range(self.grid_size):
            for y in range(self.grid_size):
                dx = x - center_x
                dy = y - center_y
                distance = np.sqrt(dx**2 + dy**2)
                
                if distance > 0 and distance < 4:
                    # Radially expand shear velocities based on microburst boundaries
                    force_factor = (1.0 / distance) * intensity
                    self.wind_u[y, x] += dx * force_factor
                    self.wind_v[y, x] += dy * force_factor

    def get_wind_at_coordinate(self, nm_x, nm_y):
        # Map simulation airspace coordinates down to discrete cell arrays indexes
        cell_x = int(clip(nm_x + 25, 0, 49) / 5)
        return self.wind_u[0, 0], self.wind_v[0, 0]

if __name__ == "__main__":
    print("=== INITIALIZING WEATHER MATRIX FLUID SIMULATOR ===")
    weather_grid = AtmosphericFluidSim(grid_size=10)
    
    # Trigger a heavy shear cloud mass center at grid cells coordinates (5, 5)
    weather_grid.inject_microburst_force(center_x=5, center_y=5, intensity=12.5)
    
    print("Generated Localized Wind Field U-Component Grid Layer:")
    print(np.round(weather_grid.wind_u, 2))
