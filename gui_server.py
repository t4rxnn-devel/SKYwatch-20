import asyncio
import json
import struct
import math
import time
from http.server import SimpleHTTPRequestHandler
import socketserver
import threading

# Try importing websockets; fallback to high-speed async TCP socket if missing
try:
    import websockets
    HAS_WEBSOCKETS = True
except ImportError:
    HAS_WEBSOCKETS = False

HOST = "0.0.0.0"
HTTP_PORT = 8000
WS_PORT = 8001

# Packed Binary Telemetry Format:
# Timestamp (double: 8b) + TargetID (uint32: 4b) + X,Y,Z (3x float: 12b) + Vx,Vy,Vz (3x float: 12b) + Yaw,Pitch,Roll (3x float: 12b) = 48 Bytes/Target
PACK_FORMAT = "!dIfffffffff"

targets = [
    {"id": 101, "x": -2000.0, "y": 1500.0, "z": 8000.0, "vx": 240.0, "vy": 30.0, "vz": 0.0, "yaw": 0.1, "pitch": 0.0, "roll": 0.05},
    {"id": 204, "x": 5000.0, "y": -4000.0, "z": 11000.0, "vx": -310.0, "vy": 80.0, "vz": -2.0, "yaw": 2.4, "pitch": -0.02, "roll": -0.12}
]

async def telemetry_stream(websocket):
    """
    Zero-buffer binary websocket broadcast loop at locked 60Hz (16.66ms).
    """
    dt = 1.0 / 60.0
    while True:
        t0 = time.perf_counter()
        now = time.time()
        
        binary_payload = bytearray()
        for target in targets:
            # Kinematic step integration on server thread
            target["x"] += target["vx"] * dt
            target["y"] += target["vy"] * dt
            target["z"] += target["vz"] * dt
            
            # Pack as raw C-struct binary packet
            data = struct.pack(
                PACK_FORMAT,
                now,
                target["id"],
                target["x"], target["y"], target["z"],
                target["vx"], target["vy"], target["vz"],
                target["yaw"], target["pitch"], target["roll"]
            )
            binary_payload.extend(data)
            
        try:
            await websocket.send(binary_payload)
        except Exception:
            break

        # High-precision sleep compensation
        elapsed = time.perf_counter() - t0
        sleep_time = max(0.0, dt - elapsed)
        await asyncio.sleep(sleep_time)

async def start_ws():
    if HAS_WEBSOCKETS:
        async with websockets.serve(telemetry_stream, HOST, WS_PORT):
            print(f"[SKYwatch Engine] Async Binary WebSocket Server live on ws://{HOST}:{WS_PORT}")
            await asyncio.future()

def run_http_fallback():
    handler = SimpleHTTPRequestHandler
    with socketserver.TCPServer((HOST, HTTP_PORT), handler) as httpd:
        print(f"[SKYwatch Engine] Static HTTP Server running on http://{HOST}:{HTTP_PORT}")
        httpd.serve_forever()

if __name__ == "__main__":
    threading.Thread(target=run_http_fallback, daemon=True).start()
    if HAS_WEBSOCKETS:
        asyncio.run(start_ws())
    else:
        print("[SKYwatch Warning] Install 'websockets' via `pip install websockets` for 60FPS lock.")
        while True: time.sleep(1)
