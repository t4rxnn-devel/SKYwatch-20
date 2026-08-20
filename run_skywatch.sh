#!/usr/bin/env bash
# SkyWatch-20 Hardened Production Execution Script
set -e

echo "📦 Setting up development environment packages..."
sudo apt-get update && sudo apt-get install -y python3-dev python3-pip libssl-dev cmake build-essential

# Verify Rust/Cargo toolchain availability
if ! command -v rustc &> /dev/null; then
    echo "🦀 Fetching Rust compiler components..."
    curl --proto '=https' --tlsv1.2 -sSf https://rustup.rs | sh -s -- -y
    source $HOME/.cargo/env
fi

echo "🏗️ Initializing CMake Native Compilation Matrix..."
mkdir -p build
cd build
cmake ..
cmake --build .

echo "🚀 Executing Integrated Multi-Language Aerospace Suite..."
./SpatialRadarExe
