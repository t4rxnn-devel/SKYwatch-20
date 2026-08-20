#!/usr/bin/env bash
# SkyWatch-20 Hardened Production Execution Script
set -e

echo "📦 Installing necessary compiler tools packages..."
mkdir -p build

if ! command -v rustc &> /dev/null; then
    echo "🦀 Fetching Rust compiler components toolchain..."
    curl --proto '=https' --tlsv1.2 -sSf https://rustup.rs | sh -s -- -y
    source $HOME/.cargo/env
fi

echo "🏗️ Building native C++ core and Rust FFI components..."
cd build
cmake ..
cmake --build .

echo "🚀 Launching Real Multi-Language Real-Time System..."
./SpatialRadarExe
