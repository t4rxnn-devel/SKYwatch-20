cmake_minimum_required(VERSION 3.16)
project(SKYwatch20 VERSION 3.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure include directory boundaries for our header files
include_directories(include)

# 1. Background custom compilation directive compiling Rust to static binaries
set(RUST_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/tcas_automation)
set(RUST_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/libtcas_automation.a)

add_custom_command(
    OUTPUT ${RUST_LIB_PATH}
    COMMAND cargo build --release
    COMMAND ${CMAKE_COMMAND} -E copy ${RUST_SRC_DIR}/target/release/libtcas_automation.a ${RUST_LIB_PATH}
    WORKING_DIRECTORY ${RUST_SRC_DIR}
    COMMENT "Compiling safe Rust safety static library..."
)

add_custom_target(tcas_rust_binary DEPENDS ${RUST_LIB_PATH})

# 2. Compile native C++ tracking execution engines
# CRITICAL FIX: We explicitly list ONLY the .cpp files here. 
# The HolyRadarCore.HC and DivineCollision.HC scripts are entirely excluded 
# from this list so standard system compilers never touch or parse them.
add_executable(SpatialRadarExe 
    src/main.cpp 
    src/ImmUkfTracker.cpp
    src/AirspaceManager.cpp
)

# Enforce synchronization boundaries inside build trees
add_dependencies(SpatialRadarExe tcas_rust_binary)

# Link everything into one clean native execution binary package
target_link_libraries(SpatialRadarExe 
    ${RUST_LIB_PATH}
    pthread 
    dl
)
