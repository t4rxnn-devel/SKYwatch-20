# --- SERVER RUNNERS & LIFECYCLE MANAGEMENT ---
def start_http_server():
    # Bind to '0.0.0.0' to accept external/localhost connections universally without refusal errors
    server = ThreadedHTTPServer(("0.0.0.0", HTTP_PORT), EnterpriseHTTPRequestHandler)
    logging.info(f"[HTTP] Enterprise Web Host listening actively on http://0.0.0.0:{HTTP_PORT}")
    try:
        server.serve_forever()
    except Exception as e:
        logging.error(f"[HTTP] Server encountered an error: {e}")

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

    # Start Async WebSocket Server bound universally to all interfaces
    start_server = websockets.serve(register_ws_client, "0.0.0.0", WS_PORT)
    loop.run_until_complete(start_server)
    logging.info(f"[WebSocket] Realtime IPC Gateway online on ws://0.0.0.0:{WS_PORT}")

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
