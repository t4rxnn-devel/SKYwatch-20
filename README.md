# 📡 SKYwatch-20

> **Tracking planes in 3D space because real airspace wasn't stressful enough already.** ✈️💥

Welcome to **SKYwatch-20**, multi-language 3D radar simulator and physics tracking sandbox. If you've ever wanted to play God with multi-million dollar stealth fighters while dodging severe cyclonic crosswinds and strict international aviation laws, you are in the right place. 

It is entirely modularized, multi-threaded, and deploys instantly via GitHub Pages.

---

## 🚀 Live Radar Console
The tactical dashboard is running live in the cloud. Launch the screen, drop in custom callsigns, set your speeds, and try not to cause a multi-million dollar mid-air collision:
👉 **https://github.io**

### 🎮 What's Happening On Screen?
* **Smooth 3D Flight Banking:** Jets don't move like rigid text characters. They use programmatic linear interpolation to physically lean and bank into turns based on their true aerodynamics.
* **Doppler Storm Failures:** The amber/orange blobs on your map are severe weather cells. Fly inside them, and localized Navier-Stokes fluid shear vectors will violently shake the aircraft's frame.
* **Phosphor Radar Sweep Line:** A hardware-accelerated GPU shader line spins gracefully through polar space, lighting up targets into a bright cyan glow before leaving an authentic fading trailing green dot grid behind.
* **TCAS Cockpit Alerts:** An automated warning ticker slams into the bottom of your screen the exact millisecond two flight trajectories breach safe separation distances.

---

## 🗂️ The Codebase Ecosystem

```text
SKYwatch-20/
├── index.html                   # The WebGL 3D Viewport & browser control panel dashboard
├── CMakeLists.txt              # Standard build pipeline linking OpenSSL cryptography bindings
├── gui_server.py                # Standalone local Python web broadcast host server
├── run_skywatch.sh              # One-click shell automator script to build the entire engine
├── include/
│   ├── app_styles.css           # CAD-styled military-green HUD and alert layout templates
│   ├── physics_solver.js        # High-order 4th-Order Runge-Kutta numerical flight tracker
│   ├── ImmUkfTracker.hpp        # Interacting Multiple Model (IMM-UKF) target tracking interface
│   ├── AirspaceManager.hpp      # AIXM 5.1 / ARINC 424 structural boundary monitor schema
│   ├── HydroPhysicsSolver.hpp   # Adaptive step-size equations and aerodynamic fluid headers
│   ├── LogExporter.hpp          # Thread-safe unalterable disk writer interface
│   ├── TimeSyncEngine.hpp       # GPS atomic clock synchronization and leap second definitions
│   ├── Link16Transceiver.hpp    # NATO STANAG 5516 Tactical Data Link packaging rules
│   └── radar_effect.frag        # GLSL per-pixel GPU fragment shader for the phosphor scan
├── src/
│   ├── ImmUkfTracker.cpp        # Unscented Transform sigma-point probability matrix math
│   ├── AirspaceManager.cpp      # Ray-casting point-in-polygon airspace area boundary checks
│   ├── HydroPhysicsSolver.cpp   # Transonic wave drag peaks and Fehlberg coefficient sub-steps
│   ├── LogExporter.cpp          # OpenSSL EVP AES-256-CBC black-box log file encrypter
│   ├── TimeSyncEngine.cpp       # Leap second drift compensation tracking loops
│   ├── Link16Transceiver.cpp    # Bit-packed J3.2 Air Track Message binary transmission serialization
│   ├── radar_telemetry.rs       # Rust high-speed state-logging data encoder
│   ├── tcas_automation.rs       # Rust automated separation check safety handler
│   ├── HolyRadarCore.HC         # Standalone TempleOS HolyC 3D core positioning code
│   └── DivineCollision.HC       # Standalone TempleOS HolyC PC-speaker warning chirp alarm
└── scripts/
    ├── vls.py                   # Python Nonlinear Vortex Lattice Method wingtip lift solver
    ├── simulate_flight_paths.m  # MATLAB kinematic flight profile matrix line plotter
    ├── process_telemetry_logs.m # MATLAB total air displacement & mean altitude log analyzer
    ├── evaluate_collision_risk.m# MATLAB Closest Point of Approach (CPA) vector predictor
    ├── filter_radar_noise.m     # MATLAB 3-point moving average sensor noise filter
    ├── simulate_wind_fields.m   # MATLAB cyclonic weather field vector map model
    ├── route_intersections.wln  # Wolfram symbolic linear parametric system cross solver
    ├── voronoi_airspace.wln     # Wolfram geometric cell Voronoi sector tessellation divider
    └── turbulence_differential.wln # Wolfram velocity decay differential equation solver
```

---

## 🛠️ Industrial Airworthiness (The Serious Engineering Bit)

To ensure distinguished experts inspecting this repository recognize its compliance(i hope so 🤞), the architecture includes a genuine aviation-standard implementations:

* **NATO Link 16 Standard (STANAG 5516):** Serializes tracking states down to a strict, bit-aligned 12-byte raw binary hardware payload (**J3.2 Air Track Messages**). No messy text strings across military radio frequencies.
* **RTCA DO-178C Level A Logic:** Automated runtime safety separation audits keep continuous tabs on minimum 5 Nautical Mile or 1,000 ft vertical altitude thresholds. 
* **WGS 84 Ellipsoidal Gravity Solver:** Accounts for Earth's true equatorial bulge using J2 zonal harmonic equations to prevent long-duration altitude-tracking calculation drift.
* **ICAO Doc 9855 Black-Box Security:** Writes encrypted data packets directly to disk using OpenSSL **AES-256-CBC blocks**, rendering logs completely tamper-proof against curious text editors.
* **Hardware-In-The-Loop (HIL):** Configures strict OPC UA network maps and 1000-microsecond cycle boundaries to simulate running on a physical laboratory testbed server rack.
