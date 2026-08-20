cmake_minimum_required(VERSION 3.16)
project(SkyWatch20Secure VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Search host platform libraries for OpenSSL Crypto resources
find_package(OpenSSL REQUIRED)

include_directories(include)

add_executable(SpatialRadarExe 
    src/main.cpp 
    src/ImmUkfTracker.cpp
    src/AirspaceManager.cpp
    src/HydroPhysicsSolver.cpp
    src/LogExporter.cpp
)

# Connect the secure crypto library directly into our tracking binary
target_link_libraries(SpatialRadarExe OpenSSL::Crypto)
