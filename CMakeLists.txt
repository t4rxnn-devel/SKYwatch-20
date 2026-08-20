cmake_minimum_required(VERSION 3.16)
project(SkyWatch20Gui VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Search host system for explicit SFML graphical library packages
find_package(SFML 2.5 REQUIRED COMPONENTS graphics window system)

include_directories(include)

add_executable(SkyWatchGuiExe 
    src/main.cpp 
    src/Aircraft.cpp 
    src/PhysicsSolver.cpp
    src/RadarGuiLayout.cpp
)

# Rigorously link SFML shared resources directly into our targeted visual binary
target_link_libraries(SkyWatchGuiExe sfml-graphics sfml-window sfml-system)
