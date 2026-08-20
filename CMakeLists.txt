cmake_minimum_required(VERSION 3.16)
project(SKYwatch20 VERSION 3.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Compile clean, functional native binaries
add_executable(SpatialRadarExe 
    src/main.cpp 
    src/ImmUkfTracker.cpp
    src/AirspaceManager.cpp
    src/HolyC_Virtual_Emulator.cpp
)

target_link_libraries(SpatialRadarExe pthread dl)
