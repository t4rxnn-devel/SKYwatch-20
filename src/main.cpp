// ============================================================================
// SkyWatch-20 Native Core Engine & IPC Interoperability Hub
// Implements Strict C++20 and Native Rust Static FFI Linkage
// ============================================================================

#include "../include/ImmUkfTracker.hpp"
#include "../include/AirspaceManager.hpp"
#include "../include/Vector3D.hpp"
#include "../include/tcas_bridge.h" // Native Rust Linkage
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <chrono>

int main() {
    std::cout << "=========================================================\n";
    std::cout << "📡 SKYWATCH-20 NATIVE BACKEND WORKER HUB RUNNING\n";
    std::cout << "=========================================================\n\n";

    AirspaceManager airspace;
    
    // Core tracking state vectors (Physical Miles NM)
    Vector3D ual_pos{-2.0, -2.0, 1.0};
    Vector3D su_pos{-1.9, -1.8, 1.1};

    // 1. Configure standard IPC TCP Socket Server to communicate with external frontend scripts
    int server_fd, client_fd;
    struct sockaddr_in address{};
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "❌ Socket creation failed.\n";
        return -1;
    }
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080); // Telemetry bus streaming on port 8080

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "❌ Bind failed. Port 8080 blocked.\n";
        return -1;
    }
    listen(server_fd, 3);
    std::cout << "📡 [IPC NETWORK]: Broadcast server online at localhost:8080\n";

    std::cout << "📥 [IPC NETWORK]: Awaiting frontend proxy connection...\n";
    // System runs headless until an isolated scripting layer attaches
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    std::cout << "✅ [IPC NETWORK]: Client proxy linked. Telemetry loop engaged.\n";

    for (int loop = 1; loop <= 5; ++loop) {
        // 2. Direct, clean evaluation of our Rust compiled static archive library
        bool safety_breached = evaluate_loss_of_separation(
            ual_pos.x, ual_pos.y, ual_pos.z * 6076.12,
            su_pos.x,  su_pos.y,  su_pos.z * 6076.12,
            5.0, 1000.0 // 5 NM, 1000 FT criteria limits
        );

        std::string alert_status = safety_breached ? "🚨 CRITICAL BREACH" : "✅ NOMINAL";
        std::cout << "[LOOP #" << loop << "] Rust FFI Safety Check: " << alert_status << "\n";

        // 3. Format telemetry data into pure JSON to pass over the IPC socket pipeline
        std::string json_packet = "{\"sweep\":" + std::to_string(loop) + 
                                  ",\"callsign\":\"SU-57\",\"x\":" + std::to_string(su_pos.x) + 
                                  ",\"y\":" + std::to_string(su_pos.y) + 
                                  ",\"conflict\":" + (safety_breached ? "true" : "false") + "}\n";

        send(client_fd, json_packet.c_str(), json_packet.length(), 0);

        su_pos.x += 0.2; su_pos.y += 0.2; // Move target
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    close(client_fd);
    close(server_fd);
    std::cout << "\n[SYSTEM INFO]: Native execution loop completed successfully.\n";
    return 0;
}
