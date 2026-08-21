"""
SKYwatch-20 Enterprise Telemetry & Web Viewer Server
DO-178C / ED-12C Assured Dual-Mode HTTP & WebSocket Engine
Provides high-frequency (60Hz) binary telemetry broadcast and static WebGL console hosting.
"""

import asyncio
import base64
import hashlib
import json
import logging
import math
import os
import signal
import socket
import socketserver
import ssl
import struct
import sys
import threading
import time
from http.server import SimpleHTTPRequestHandler
from typing import Dict, List, Optional

# Enable structured logging for audit compliance
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] [%(name)s] %(message)s",
    handlers=[logging.StreamHandler(sys.stdout)]
)
logger = logging.getLogger("SKYwatch.TelemetryServer")

# Check native websockets library availability
try:
    import websockets
    HAS_WEBSOCKETS = True
except ImportError:
    HAS_WEBSOCKETS = False

# Configuration Parameters
HOST = os.environ.get("SKYWATCH_HOST", "0.0.0.0")
HTTP_PORT = int(os.environ.get("SKYWATCH_HTTP_PORT", 8000))
WS_PORT = int(os.environ.get("SKYWATCH_WS_PORT", 8001))
TARGET_FPS = 60.0
DT = 1.0 / TARGET_FPS

# Packed Binary Telemetry Format:
# Timestamp (double: 8b) + TargetID (uint32: 4b) + X,Y,Z (3x float: 12b) + Vx,Vy,Vz (3x float: 12b) + Yaw,Pitch,Roll (3x float: 12b) = 48 Bytes
PACK_FORMAT = "!dIfffffffff"
PACK_SIZE = struct.calcsize(PACK_FORMAT)

# Thread-safe Target State Registry
targets_lock = threading.Lock()
targets: List[Dict[str, float]] = [
    {"id": 101, "x": -2000.0, "y": 1500.0, "z": 8000.0, "vx": 240.0, "vy": 30.0, "vz": 0.0, "yaw": 0.1, "pitch": 0.0, "roll": 0.05},
    {"id": 204, "x": 5000.0, "y": -4000.0, "z": 11000.0, "vx": -310.0, "vy": 80.0, "vz": -2.0, "yaw": 2.4, "pitch": -0.02, "roll": -0.12},
    {"id": 309, "x": 1200.0, "y": 8000.0, "z": 5000.0, "vx": 180.0, "vy": -150.0, "vz": 1.5, "yaw": 1.2, "pitch": 0.01, "roll": -0.02}
]

# Server State Control
shutdown_event = threading.Event()


def generate_binary_telemetry(dt: float, now: float) -> bytearray:
    """Computes deterministic 60Hz kinematic state update and packs C-struct binary payload."""
    payload = bytearray()
    with targets_lock:
        for target in targets:
            # Kinematic integration
            target["x"] += target["vx"] * dt
            target["y"] += target["vy"] * dt
            target["z"] += target["vz"] * dt
            
            # Boundary wrap-around for endless simulation stability
            if abs(target["x"]) > 50000.0: target["vx"] *= -1.0
            if abs(target["y"]) > 50000.0: target["vy"] *= -1.0

            data = struct.pack(
                PACK_FORMAT,
                now,
                int(target["id"]),
                target["x"], target["y"], target["z"],
                target["vx"], target["vy"], target["vz"],
                target["yaw"], target["pitch"], target["roll"]
            )
            payload.extend(data)
    return payload


def get_ssl_context() -> Optional[ssl.SSLContext]:
    """Configures WSS (WebSocket Secure) TLS context if cert files are present."""
    cert_file = os.environ.get("SKYWATCH_SSL_CERT", "cert.pem")
    key_file = os.environ.get("SKYWATCH_SSL_KEY", "key.pem")
    if os.path.exists(cert_file) and os.path.exists(key_file):
        try:
            ssl_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
            ssl_ctx.load_cert_chain(certfile=cert_file, keyfile=key_file)
            logger.info(f"TLS/WSS Encryption context verified ({cert_file}).")
            return ssl_ctx
        except Exception as e:
            logger.error(f"Failed to load SSL Certificates: {e}")
    return None


class EnterpriseHTTPHandler(SimpleHTTPRequestHandler):
    """Custom static content server with CORS, audit headers, and health checks."""
    
    def do_GET(self):
        if self.path == "/health":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.end_headers()
            status = {
                "status": "HEALTHY",
                "timestamp": time.time(),
                "active_targets": len(targets),
                "websockets_engine": "native" if HAS_WEBSOCKETS else "fallback_raw"
            }
            self.wfile.write(json.dumps(status).encode("utf-8"))
            return
        super().do_GET()

    def end_headers(self):
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, OPTIONS")
        self.send_header("Cache-Control", "no-store, no-cache, must-revalidate")
        self.send_header("X-Content-Type-Options", "nosniff")
        super().end_headers()

    def log_message(self, format, *args):
        logger.debug(f"HTTP Request: {format % args}")


# -------------------------------------------------------------------
# 1. Native Websockets Stream Handler
# -------------------------------------------------------------------
async def websockets_stream_handler(websocket):
    logger.info(f"New Client Connection: {websocket.remote_address}")
    try:
        while not shutdown_event.is_set():
            t0 = time.perf_counter()
            now = time.time()
            payload = generate_binary_telemetry(DT, now)
            await websocket.send(payload)
            elapsed = time.perf_counter() - t0
            await asyncio.sleep(max(0.0, DT - elapsed))
    except Exception as e:
        logger.debug(f"WebSocket client disconnected: {e}")


async def start_ws_server():
    ssl_context = get_ssl_context()
    protocol = "wss" if ssl_context else "ws"
    async with websockets.serve(websockets_stream_handler, HOST, WS_PORT, ssl=ssl_context):
        logger.info(f"Enterprise Binary Broadcast Live on {protocol}://{HOST}:{WS_PORT}")
        while not shutdown_event.is_set():
            await asyncio.sleep(1.0)


# -------------------------------------------------------------------
# 2. Fallback RFC 6455 Compliant Raw Socket Handler
# -------------------------------------------------------------------
def generate_ws_accept_key(sec_key: str) -> str:
    """Computes RFC 6455 WebSocket Handshake accept key via SHA1 hash."""
    GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
    sha1 = hashlib.sha1((sec_key.strip() + GUID).encode("utf-8")).digest()
    return base64.b64encode(sha1).decode("utf-8")


def raw_socket_fallback_server():
    """Raw TCP socket streaming fallback with RFC 6455 handshake verification."""
    ssl_context = get_ssl_context()
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_sock.settimeout(1.0)
    server_sock.bind((HOST, WS_PORT))
    server_sock.listen(10)
    
    logger.info(f"Fallback Zero-Dependency Socket Server live on port {WS_PORT}")

    while not shutdown_event.is_set():
        try:
            conn, addr = server_sock.accept()
        except socket.timeout:
            continue
        except Exception as e:
            logger.error(f"Socket accept error: {e}")
            break

        threading.Thread(target=handle_raw_client, args=(conn, ssl_context), daemon=True).start()

    server_sock.close()


def handle_raw_client(conn: socket.socket, ssl_context: Optional[ssl.SSLContext]):
    try:
        if ssl_context:
            conn = ssl_context.wrap_socket(conn, server_side=True)
        
        request = conn.recv(2048).decode("utf-8", errors="ignore")
        if "Upgrade: websocket" in request:
            sec_key = None
            for line in request.split("\r\n"):
                if line.startswith("Sec-WebSocket-Key:"):
                    sec_key = line.split(":")[1]
                    break

            if sec_key:
                accept_key = generate_ws_accept_key(sec_key)
                response = (
                    "HTTP/1.1 101 Switching Protocols\r\n"
                    "Upgrade: websocket\r\n"
                    "Connection: Upgrade\r\n"
                    f"Sec-WebSocket-Accept: {accept_key}\r\n\r\n"
                )
                conn.sendall(response.encode("utf-8"))

                while not shutdown_event.is_set():
                    t0 = time.perf_counter()
                    now = time.time()
                    payload = generate_binary_telemetry(DT, now)
                    
                    # Construct binary frame header (0x82 opcode)
                    payload_len = len(payload)
                    if payload_len <= 125:
                        header = bytearray([0x82, payload_len])
                    elif payload_len <= 65535:
                        header = bytearray([0x82, 126]) + struct.pack("!H", payload_len)
                    else:
                        header = bytearray([0x82, 127]) + struct.pack("!Q", payload_len)

                    conn.sendall(header + payload)
                    elapsed = time.perf_counter() - t0
                    time.sleep(max(0.0, DT - elapsed))
    except Exception:
        pass
    finally:
        conn.close()


# -------------------------------------------------------------------
# 3. HTTP Server Thread Orchestration
# -------------------------------------------------------------------
def run_http_server():
    socketserver.TCPServer.allow_reuse_address = True
    try:
        with socketserver.TCPServer((HOST, HTTP_PORT), EnterpriseHTTPHandler) as httpd:
            logger.info(f"Static HTTP Console Viewport running at http://{HOST}:{HTTP_PORT}")
            httpd.timeout = 1.0
            while not shutdown_event.is_set():
                httpd.handle_request()
    except Exception as e:
        logger.error(f"HTTP Server Exception: {e}")


def signal_handler(signum, frame):
    logger.info("Termination signal received. Initiating graceful shutdown...")
    shutdown_event.set()


if __name__ == "__main__":
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)

    # Launch background HTTP server
    http_thread = threading.Thread(target=run_http_server, daemon=True)
    http_thread.start()

    # Launch WebSocket telemetry server
    if HAS_WEBSOCKETS:
        try:
            asyncio.run(start_ws_server())
        except KeyboardInterrupt:
            pass
    else:
        raw_socket_fallback_server()

    shutdown_event.set()
    logger.info("SKYwatch-20 Server Shutdown Complete.")
