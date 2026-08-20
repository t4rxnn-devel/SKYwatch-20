#!/usr/bin/env bash
# SkyWatch-20 Hardened Production Execution Script
set -e

echo "📦 Upgrading systems automation development packages..."
mkdir -p build

# Check for Rust compiler installation
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
echo "🚀 Launching Integrated Multi-Language Aerospace Suite..."
./SpatialRadarExe
