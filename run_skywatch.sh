#!/usr/bin/env bash
# SkyWatch-20 Hardened Decoupled Toolchain Execution Architecture
set -e

echo "================================================================="
echo "🏗️  SKYWATCH-20 ISOLATED TOOLCHAIN COMPILATION MATRIX INITIALIZED"
echo "================================================================="

# 1. Environment Package Verification
sudo apt-get update && sudo apt-get install -y python3-dev python3-pip libssl-dev cmake build-essential

if ! command -v cargo &> /dev/null; then
    echo "🦀 Fetching isolated Rust toolchain compiler..."
    curl --proto '=https' --tlsv1.2 -sSf https://rustup.rs | sh -s -- -y
    source $HOME/.cargo/env
fi

# 2. Build the C++ and Rust Integrated Binary Package Natively
mkdir -p build && cd build
cmake ..
cmake --build .

echo "✅ Native Tracking Binary Compiled Successfully."
echo ""

# 3. Handle Polyglot Analytics Scripts strictly as Isolated Processes
echo "📊 [ANALYTICS INITIALIZATION]: Running standalone out-of-process workers..."

# Launch Python VLS wing aerodynamics separately as an isolated background task
if [ -f "../scripts/vls.py" ]; then
    echo "   -> [PYTHON]: Launching scripts/vls.py worker process..."
    python3 ../scripts/vls.py &
fi

# Launch MATLAB/Octave routines headless as an isolated background task
if command -v octave &> /dev/null; then
    echo "   -> [MATLAB/OCTAVE]: Running scripts/filter_radar_noise.m asynchronously..."
    octave --headless ../scripts/filter_radar_noise.m &
elif command -v matlab &> /dev/null; then
    echo "   -> [MATLAB]: Running scripts/filter_radar_noise.m asynchronously..."
    matlab -batch "run('../scripts/filter_radar_noise.m'); exit;" &
else
    echo "   -> [ANALYTICS NOTICE]: Headless MATLAB/Octave installation missing. Skipping calculation worker."
fi

echo ""
echo "================================================================="
echo "🚀 EXECUTING LOCAL TELEMETRY PIPELINE STREAM"
echo "================================================================="
# Boot up the clean native C++ binary execution hub
./SpatialRadarExe
