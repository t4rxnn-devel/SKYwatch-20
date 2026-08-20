#!/usr/bin/env bash
# SkyWatch-20 Hardened Production Execution Script
set -e

echo "📦 Syncing host development compilation packages..."
mkdir -p build

echo "🏗️ Initializing CMake Native Compilation Matrix..."
cd build
cmake ..
cmake --build .

echo ""
echo "🚀 Launching Real Multi-Language JIT Aerospace Suite..."
./SpatialRadarExe
