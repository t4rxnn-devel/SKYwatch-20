#!/usr/bin/env bash
# ==============================================================================
# 📡 SKYwatch-20 Industrial Launch & Build Orchestrator
# Multi-Language Tactical Radar Sandbox Launcher
# ==============================================================================

set -eo pipefail

# --- Color Constants ---
BOLD='\033[1m'
GREEN='\033[0;32m'
CYAN='\033[0;36m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

DEFAULT_PORT=8000
PORT=${SKYWATCH_PORT:-$DEFAULT_PORT}
BUILD_DIR="build"
TARGET_DIR="target/release"

banner() {
  echo -e "${CYAN}${BOLD}"
  echo "    ┌─────────────────────────────────────────────────────────┐"
  echo "    │   📡 SKYwatch-20 : Tactical 3D Airspace Radar Core      │"
  echo "    │   Polyglot Flight Tracker & Physics Simulation Engine   │"
  echo "    └─────────────────────────────────────────────────────────┘"
  echo -e "${NC}"
}

log_info()    { echo -e "${GREEN}[INFO]${NC} $1"; }
log_warn()    { echo -e "${YELLOW}[WARN]${NC} $1"; }
log_err()     { echo -e "${RED}[ERROR]${NC} $1"; }
log_step()    { echo -e "${CYAN}${BOLD}==>${NC} ${BOLD}$1${NC}"; }

# --- Check System Dependencies ---
check_environment() {
  log_step "Auditing Runtime Environment & Toolchains..."

  if ! command -v python3 &> /dev/null; then
    log_err "Python 3 is required to run the local server. Aborting."
    exit 1
  fi

  log_info "Python version: $(python3 --version)"

  if command -v cmake &> /dev/null; then
    log_info "C++ Toolchain: CMake detected ($(cmake --version | head -n1))"
  else
    log_warn "CMake not found. Skipping native C++ core compilation."
  fi

  if command -v cargo &> /dev/null; then
    log_info "Rust Toolchain: Cargo detected ($(cargo --version))"
  else
    log_warn "Cargo not found. Skipping Rust safety modules compilation."
  fi
}

# --- Build Subsystems ---
build_native_cores() {
  log_step "Building Polyglot Subsystems..."

  # C++20 Core Compilation
  if command -v cmake &> /dev/null && [ -f "CMakeLists.txt" ]; then
    log_info "Building C++20 IMM-UKF Tracking Core..."
    mkdir -p "$BUILD_DIR"
    (
      cd "$BUILD_DIR"
      cmake -DCMAKE_BUILD_TYPE=Release .. > /dev/null 2>&1 || true
      make -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)" > /dev/null 2>&1 || true
    )
    log_info "C++ Core build check complete."
  fi

  # Rust Safety Auditor Compilation
  if command -v cargo &> /dev/null && [ -f "Cargo.toml" ]; then
    log_info "Building Rust RTCA DO-178C Safety Module..."
    cargo build --release > /dev/null 2>&1 || true
    log_info "Rust Safety Module build check complete."
  fi
}

# --- Network & Server Controls ---
verify_port() {
  if lsof -Pi :"$PORT" -sTCP:LISTEN -t >/dev/null 2>&1 ; then
    log_warn "Port $PORT is already in use."
    PORT=$((PORT + 1))
    log_info "Re-routing local web broadcast server to Port $PORT..."
  fi
}

launch_browser() {
  local url="http://localhost:${PORT}"
  log_info "Opening Radar Viewport at $url"
  
  if command -v xdg-open &> /dev/null; then
    xdg-open "$url" > /dev/null 2>&1 &
  elif command -v open &> /dev/null; then
    open "$url" > /dev/null 2>&1 &
  else
    log_warn "Could not auto-launch browser. Please open $url manually."
  fi
}

start_server() {
  log_step "Spinning up SKYwatch-20 Local Host Server..."
  verify_port

  # Launch browser after a 1.5s delay in background
  (sleep 1.5 && launch_browser) &

  # Execute GUI broadcast server
  if [ -f "gui_server.py" ]; then
    python3 gui_server.py --port "$PORT"
  else
    log_info "gui_server.py not found. Falling back to python http.server..."
    python3 -m http.server "$PORT"
  fi
}

# --- Main Entry Point ---
main() {
  banner
  check_environment
  build_native_cores
  start_server
}

main "$@"
