cmake_minimum_required(VERSION 3.16)
project(SKYwatch20 VERSION 3.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# 1. Look for OpenSSL without forcing a critical build failure if missing
find_package(OpenSSL QUIET)

add_executable(SpatialRadarExe 
    src/main.cpp 
    src/ImmUkfTracker.cpp
    src/AirspaceManager.cpp
)

# 2. Dynamic linking configuration based on platform capabilities
if(OPENSSL_FOUND)
    message(STATUS "🔒 [BUILD]: OpenSSL detected. Hardened AES-256 binary encryption enabled.")
    include_directories(${OPENSSL_INCLUDE_DIR})
    target_link_libraries(SpatialRadarExe OpenSSL::Crypto)
else()
    message(WARNING "⚠️ [BUILD]: OpenSSL missing. Running tracking scripts under nominal unencrypted fallback logs.")
endif()

target_link_libraries(SpatialRadarExe pthread dl)
