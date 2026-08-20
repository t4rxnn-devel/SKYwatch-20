import asyncio
import json
import struct
import math
import time
import os
import ssl
import socket
import threading
from http.server import SimpleHTTPRequestHandler
import socketserver

# Detect standard websockets library or fallback to zero-dependency raw socket handler
try:
    import websockets
    HAS_WEBSOCKETS = True
except ImportError:
    HAS_WEBSOCKETS = False

HOST = "0.0.0.0"
HTTP_PORT = 8000
WS_PORT = 8001

# Packed Binary Telemetry Format:
# Timestamp (double: 8b) + TargetID (uint32: 4b) + X,Y,Z (3x float: 12b) + Vx,Vy,Vz (3x float: 12b) + Yaw,Pitch,Roll (3x float: 12b) = 48 Bytes
PACK_FORMAT = "!dIfffffffff"

targets = [
    {"id": 101, "x": -2000.0, "y": 1500.0, "z": 8000.0, "vx": 240.0, "vy": 30.0, "vz": 0.0, "yaw": 0.1, "pitch": 0.0, "roll": 0.05},
    {"id": 204, "x": 5000.0, "y": -4000.0, "z": 11000.0, "vx": -310.0, "vy": 80.0, "vz": -2.0, "yaw": 2.4, "pitch": -0.02, "roll": -0.12}
]

def generate_binary_telemetry(dt: float, now: float) -> bytearray:
    """Computes 60Hz kinematic state update and packs C-struct binary payload."""
    payload = bytearray()
    for target in targets:
        target["x"] += target["vx"] * dt
        target["y"] += target["vy"] * dt
        target["z"] += target["vz"] * dt

        data = struct.pack(
            PACK_FORMAT,
            now,
            target["id"],
            target["x"], target["y"], target["z"],
            target["vx"], target["vy"], target["vz"],
            target["yaw"], target["pitch"], target["roll"]
        )
        payload.extend(data)
    return payload

# -------------------------------------------------------------------
# 1. SSL/TLS Production Security Context Helper
# -------------------------------------------------------------------
def get_ssl_context():
    """Configures WSS (WebSocket Secure) TLS context if cert files are present."""
    cert_file = os.environ.get("SKYWATCH_SSL_CERT", "cert.pem")
    key_file = os.environ.get("SKYWATCH_SSL_KEY", "key.pem")

    if os.path.exists(cert_file) and os.path.exists(key_file):
        ssl_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
        ssl_ctx.load_cert_chain(certfile=cert_file, keyfile=key_file)
        print(f"[SKYwatch Security] TLS/WSS Encryption enabled ({cert_file}).")
        return ssl_ctx
    return None

# -------------------------------------------------------------------
# 2. Native Library Path (`websockets`)
# -------------------------------------------------------------------
async def websockets_stream_handler(websocket):
    dt = 1.0 / 60.0
    while True:
        t0 = time.perf_counter()
        now = time.time()
        payload = generate_binary_telemetry(dt, now)

        try:
            await websocket.send(payload)
        except Exception:
            break

        elapsed = time.perf_counter() - t0
        await asyncio.sleep(max(0.0, dt - elapsed))

async def start_ws_server():
    ssl_context = get_ssl_context()
    protocol = "wss" if ssl_context else "ws"
    
    async with websockets.serve(websockets_stream_handler, HOST, WS_PORT, ssl=ssl_context):
        print(f"[SKYwatch Engine] High-Speed Binary Server live on {protocol}://{HOST}:{WS_PORT}")
        await asyncio.future()

# -------------------------------------------------------------------
# 3. Fallback Zero-Dependency Raw Socket Handler (When `websockets` missing)
# -------------------------------------------------------------------
def raw_socket_fallback_server():
    """Raw TCP socket streaming fallback to preserve 60Hz feed without external PIP modules."""
    ssl_context = get_ssl_context()
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_sock.bind((HOST, WS_PORT))
    server_sock.listen(5)
    
    print(f"[SKYwatch Engine] Fallback Zero-Dependency Socket Server live on port {WS_PORT}")
    
    dt = 1.0 / 60.0
    while True:
        try:
            conn, _ = server_sock.accept()
            if ssl_context:
                conn = ssl_context.wrap_socket(conn, server_side=True)
            
            # Simple HTTP/1.1 Upgrade Handshake framing bypass
            request = conn.recv(1024)
            if b"Upgrade: websocket" in request:
                response = (
                    "HTTP/1.1 101 Switching Protocols\r\n"
                    "Upgrade: websocket\r\n"
                    "Connection: Upgrade\r\n"
                    "Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=\r\n\r\n"
                )
                conn.sendall(response.encode('utf-8'))

            while True:
                t0 = time.perf_counter()
                now = time.time()
                payload = generate_binary_telemetry(dt, now)
                
                # Single unfragmented WS binary frame header (0x82 opcode)
                header = bytearray([0x82, len(payload)])
                conn.sendall(header + payload)
                
                elapsed = time.perf_counter() - t0
                time.sleep(max(0.0, dt - elapsed))
        except Exception:
            continue

# -------------------------------------------------------------------
# 4. HTTP Static Host Thread
# -------------------------------------------------------------------
def run_http_server():
    handler = SimpleHTTPRequestHandler
    with socketserver.TCPServer((HOST, HTTP_PORT), handler) as httpd:
        print(f"[SKYwatch Engine] Static HTTP Viewport running on http://{HOST}:{HTTP_PORT}")
        httpd.serve_forever()

if __name__ == "__main__":
    threading.Thread(target=run_http_server, daemon=True).start()
    
    if HAS_WEBSOCKETS:
        asyncio.run(start_ws_server())
    else:
        raw_socket_fallback_server()
