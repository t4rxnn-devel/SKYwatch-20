cmake_minimum_required(VERSION 3.16)
project(SKYwatch20 VERSION 3.5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# 1. Locate Embedded Python C-API libraries parameters
find_package(Python3 COMPONENTS Development REQUIRED)
include_directories(${Python3_INCLUDE_DIRS})

# 2. Compile and pull the native Rust Static C-Archive package natively via Cargo
set(RUST_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/aerospace_modules)
set(RUST_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/libaerospace_modules.a)

add_custom_command(
    OUTPUT ${RUST_LIB_PATH}
    COMMAND cargo build --release
    COMMAND ${CMAKE_COMMAND} -E copy ${RUST_SRC_DIR}/target/release/libaerospace_modules.a ${RUST_LIB_PATH}
    WORKING_DIRECTORY ${RUST_SRC_DIR}
    COMMENT "Compiling safe Rust RTCA DO-178C static library archive..."
)

add_custom_target(aerospace_rust_package DEPENDS ${RUST_LIB_PATH})

# 3. Weave all compiled modules into your central machine binary
add_executable(SpatialRadarExe 
    src/main.cpp 
    src/ImmUkfTracker.cpp
    src/AirspaceManager.cpp
)

add_dependencies(SpatialRadarExe aerospace_rust_package)

target_link_libraries(SpatialRadarExe 
    ${RUST_LIB_PATH}
    Python3::Development
    pthread 
    dl
)
