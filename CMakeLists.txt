cmake_minimum_required(VERSION 3.16)
project(SpatialConsoleRadar VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(SpatialRadarExe 
    src/main.cpp 
    src/Aircraft.cpp 
    src/RadarSystem.cpp
)
