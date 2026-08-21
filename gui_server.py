import http.server
import socketserver
import os
import sys

# Ensure current working directory always targets the script root
ROOT_DIR = os.path.dirname(os.path.abspath(__file__))
os.chdir(ROOT_DIR)

PORT = 8000

class EngineHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        # Enable CORS for WebGL/WASM assets
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
        super().end_headers()

if __name__ == "__main__":
    socketserver.TCPServer.allow_reuse_address = True
    with socketserver.TCPServer(("", PORT), EngineHTTPRequestHandler) as httpd:
        print(f"[*] SKYwatch-20 Radar Server active on http://localhost:{PORT}")
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n[-] Shutting down server gracefully.")
            httpd.shutdown()
