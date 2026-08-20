#!/usr/bin/env bash
# SkyWatch-20 Hardened Production Execution Script
set -e

echo "📦 Organizing host development compilation packages..."
mkdir -p build

if ! command -v rustc &> /dev/null; then
    echo "🦀 Fetching Rust compiler components toolchain..."
    curl --proto '=https' --tlsv1.2 -sSf https://rustup.rs | sh -s -- -y
    source $HOME/.cargo/env
fi

echo "🏗️ Initializing CMake Native Compilation Matrix..."
cd build
cmake ..
cmake --build .

echo ""
echo "🚀 Launching Real Multi-Language Aerospace Suite over Socket Buses..."
./SpatialRadarExe
