#!/usr/bin/env python3
"""
SKYwatch-20 Mission Control Host & IPC Telemetry Bridge
Enterprise-Grade Hybrid Server (Async WebSockets + HTTP Static Server + Native Subprocess Orchestrator)
"""

import asyncio
import json
import logging
import os
import signal
import sys
import threading
import time
from http.server import HTTPServer, SimpleHTTPRequestHandler
from socketserver import ThreadingMixIn
import websockets

# --- CONFIGURATION & PATH INITIALIZATION ---
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
os.chdir(ROOT_DIR)

HTTP_PORT = 8000
WS_PORT = 8080
LOG_FORMAT = "%(asctime)s [%(levelname)s] (%(threadName)s) %(message)s"
logging.basicConfig(level=logging.INFO, format=LOG_FORMAT)

# Global Telemetry State & Client Registry
CONNECTED_WS_CLIENTS = set()
TELEMETRY_STATE = {
    "system_status": "ONLINE",
    "active_tracks": [],
    "tcas_alerts": [],
    "telemetry_tick": 0,
    "last_update": time.time()
}
SUBPROCESSES = []
RUNNING = True


# --- HIGH-PERFORMANCE MULTI-THREADED HTTP SERVER ---
class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    """Handles HTTP requests asynchronously across threads."""
    daemon_threads = True


class EnterpriseHTTPRequestHandler(SimpleHTTPRequestHandler):
    """Custom HTTP Handler providing CORS, WASM headers, and REST API endpoints."""

    def end_headers(self):
        # Allow Cross-Origin Requests & Enable Cross-Origin Isolation for WASM SharedArrayBuffer
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.send_header("Cross-Origin-Opener-Policy", "same-origin")
        self.send_header("Cross-Origin-Embedder-Policy", "require-corp")
        self.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
        super().end_headers()

    def do_OPTIONS(self):
        self.send_response(200)
        self.end_headers()

    def do_GET(self):
        # REST API endpoint for system health & telemetry polling fallback
        if self.path == "/api/status":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            payload = json.dumps(TELEMETRY_STATE).encode("utf-8")
            self.wfile.write(payload)
            return
        
        if self.path == "/api/health":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            self.wfile.write(json.dumps({"status": "HEALTHY", "clients": len(CONNECTED_WS_CLIENTS)}).encode("utf-8"))
            return

        super().do_GET()

    def do_POST(self):
        # REST API for incoming command dispatches
        if self.path == "/api/command":
            content_length = int(self.headers.get("Content-Length", 0))
            body = self.rfile.read(content_length).decode("utf-8")
            try:
                command_data = json.loads(body)
                logging.info(f"[REST Command Received] {command_data}")
                self.send_response(200)
                self.send_header("Content-Type", "application/json")
                self.end_headers()
                self.wfile.write(json.dumps({"status": "SUCCESS", "command": command_data}).encode("utf-8"))
            except Exception as e:
                self.send_response(400)
                self.end_headers()
                self.wfile.write(json.dumps({"error": str(e)}).encode("utf-8"))
            return


# --- WEBSOCKET IPC & TELEMETRY BROADCASTER ---
async def register_ws_client(websocket):
    CONNECTED_WS_CLIENTS.add(websocket)
    logging.info(f"[WebSocket] Client connected: {websocket.remote_address}. Total: {len(CONNECTED_WS_CLIENTS)}")
    try:
        # Send initial handshake state
        await websocket.send(json.dumps({"type": "INIT_STATE", "data": TELEMETRY_STATE}))
        async for message in websocket:
            try:
                msg_data = json.loads(message)
                logging.info(f"[WebSocket Received] {msg_data}")
                # Echo or process incoming socket commands from WebGL UI
                await broadcast_telemetry({"type": "ACK_CMD", "payload": msg_data})
            except json.JSONDecodeError:
                logging.warning("[WebSocket] Received non-JSON payload.")
    except websockets.exceptions.ConnectionClosedError:
        pass
    finally:
        CONNECTED_WS_CLIENTS.remove(websocket)
        logging.info(f"[WebSocket] Client disconnected: {websocket.remote_address}")


async def broadcast_telemetry(data):
    if CONNECTED_WS_CLIENTS:
        message = json.dumps(data)
        await asyncio.gather(*[client.send(message) for client in CONNECTED_WS_CLIENTS if client.open], return_exceptions=True)


# --- NATIVE SUBPROCESS & POLYGLOT PIPELINE MONITOR ---
def spawn_native_engine(command, name):
    """Spawns background native binaries (C++ / Rust / Python) and reads stdout pipe."""
    global RUNNING
    try:
        proc = os.subprocess.Popen(
            command,
            stdout=os.subprocess.PIPE,
            stderr=os.subprocess.PIPE,
            text=True,
            bufsize=1
        )
        SUBPROCESSES.append(proc)
        logging.info(f"[Engine] Spawned {name} (PID: {proc.pid})")

        while RUNNING and proc.poll() is None:
            line = proc.stdout.readline()
            if line:
                line_str = line.strip()
                if line_str.startswith("{") and line_str.endswith("}"):
                    try:
                        parsed = json.loads(line_str)
                        TELEMETRY_STATE["active_tracks"] = parsed.get("tracks", TELEMETRY_STATE["active_tracks"])
                        TELEMETRY_STATE["tcas_alerts"] = parsed.get("alerts", TELEMETRY_STATE["tcas_alerts"])
                    except json.JSONDecodeError:
                        pass
    except Exception as e:
        logging.warning(f"[Engine] Could not start {name}: {e}. Operating in standalone mock mode.")


def telemetry_generator_loop(loop):
    """Simulates/aggregates realtime radar feeds if native binaries are compiling/absent."""
    global TELEMETRY_STATE, RUNNING
    tick = 0
    while RUNNING:
        time.sleep(0.1) # 10Hz Telemetry Broadcast
        tick += 1
        TELEMETRY_STATE["telemetry_tick"] = tick
        TELEMETRY_STATE["last_update"] = time.time()
        
        payload = {
            "type": "TELEMETRY_FRAME",
            "tick": tick,
            "timestamp": time.time(),
            "tracks": TELEMETRY_STATE["active_tracks"],
            "alerts": TELEMETRY_STATE["tcas_alerts"]
        }
        
        asyncio.run_coroutine_threadsafe(broadcast_telemetry(payload), loop)


# --- SERVER RUNNERS & LIFECYCLE MANAGEMENT ---
def start_http_server():
    server = ThreadedHTTPServer(("", HTTP_PORT), EnterpriseHTTPRequestHandler)
    logging.info(f"[HTTP] Enterprise Web Host listening on http://localhost:{HTTP_PORT}")
    try:
        server.serve_forever()
    except Exception:
        pass


def main():
    global RUNNING
    logging.info("[*] Starting SKYwatch-20 Enterprise Server Matrix...")

    # 1. Start Multi-threaded HTTP Server in Background
    http_thread = threading.Thread(target=start_http_server, name="HTTP-Server-Thread", daemon=True)
    http_thread.start()

    # 2. Check and spawn C++ tracking core if binary exists
    cpp_binary = os.path.join(ROOT_DIR, "build", "skywatch_core")
    if os.path.exists(cpp_binary):
        cpp_thread = threading.Thread(target=spawn_native_engine, args=([cpp_binary], "C++ IMM-UKF Core"), daemon=True)
        cpp_thread.start()

    # 3. Setup Asyncio Event Loop for WebSocket Server & Telemetry Pipelines
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)

    # Start 10Hz telemetry sync thread
    telemetry_thread = threading.Thread(target=telemetry_generator_loop, args=(loop,), name="Telemetry-Generator", daemon=True)
    telemetry_thread.start()

    # Start Async WebSocket Server
    start_server = websockets.serve(register_ws_client, "0.0.0.0", WS_PORT)
    loop.run_until_complete(start_server)
    logging.info(f"[WebSocket] Realtime IPC Gateway online on ws://localhost:{WS_PORT}")

    # Graceful Shutdown Signal Handler
    def shutdown_handler(sig, frame):
        global RUNNING
        logging.info("[-] Shutdown signal received. Terminating processes...")
        RUNNING = False
        for proc in SUBPROCESSES:
            try:
                proc.terminate()
            except Exception:
                pass
        sys.exit(0)

    signal.signal(signal.SIGINT, shutdown_handler)
    signal.signal(signal.SIGTERM, shutdown_handler)

    try:
        loop.run_forever()
    except KeyboardInterrupt:
        shutdown_handler(None, None)


if __name__ == "__main__":
    main()
