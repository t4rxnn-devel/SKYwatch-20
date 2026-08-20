#!/bin/bash
# SkyWatch-20 Master Orchestration Script

echo "============== Building SkyWatch-20 Ecosystem =============="

# 1. Clean CMake compilation steps for the core C++ engine
mkdir -p build && cd build
cmake .. && cmake --build .

if [ $? -eq 0 ]; then
    echo "✅ Native binary generated cleanly."
    echo "🚀 Starting primary control tower visualization mapping..."
    # 2. Automatically spin up the app with zero extra typing flags needed
    ./SpatialRadarExe
else
    echo "❌ Error building system files."
    exit 1
fi
