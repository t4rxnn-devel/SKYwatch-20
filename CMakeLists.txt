cmake_minimum_required(VERSION 3.16)
project(SKYwatch20 VERSION 4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# 1. Isolate the Rust compilation target into a separate static binary path
set(RUST_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/tcas_automation)
set(RUST_LIB_PATH ${CMAKE_CURRENT_BINARY_DIR}/libtcas_automation.a)

add_custom_command(
    OUTPUT ${RUST_LIB_PATH}
    COMMAND cargo build --release
    COMMAND ${CMAKE_COMMAND} -E copy ${RUST_SRC_DIR}/target/release/libtcas_automation.a ${RUST_LIB_PATH}
    WORKING_DIRECTORY ${RUST_SRC_DIR}
    COMMENT "Building isolated Rust FFI safety library..."
)

add_custom_target(tcas_rust_binary DEPENDS ${RUST_LIB_PATH})

# 2. Compile native C++ tracking execution engines
# NOTICE: .HC, .py, .m, and .wln files are strictly omitted from this target.
add_executable(SpatialRadarExe
    src/main.cpp
    src/ImmUkfTracker.cpp
    src/AirspaceManager.cpp
)

# Enforce clean sequential dependency building
add_dependencies(SpatialRadarExe tcas_rust_binary)

# Link the valid compiled binary objects natively
target_link_libraries(SpatialRadarExe
    ${RUST_LIB_PATH}
    pthread
    dl
)
