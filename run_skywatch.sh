#!/usr/bin/env bash
# SkyWatch-20 Hardened Production Execution Script
set -e

echo "📦 Syncing development environment packages..."
sudo apt-get update && sudo apt-get install -y python3-dev python3-pip libssl-dev cmake build-essential

if ! command -v rustc &> /dev/null; then
    echo "🦀 Fetching Rust compiler components toolchain..."
    curl --proto '=https' --tlsv1.2 -sSf https://rustup.rs | sh -s -- -y
    source $HOME/.cargo/env
fi

echo "🏗️ Initializing Package Matrix Compilation..."
mkdir -p build && cd build
cmake ..
cmake --build .

echo ""
echo "🚀 Launching Real Multi-Language Aerospace Suite Package..."
./SpatialRadarExe
