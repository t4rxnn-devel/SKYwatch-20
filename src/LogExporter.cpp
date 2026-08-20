#include "LogExporter.hpp"
#include <iostream>

LogExporter::LogExporter(std::string output_path) noexcept 
    : m_target_filename(std::move(output_path)) {}

LogExporter::~LogExporter() {
    execute_disk_export(); // Self-executing data preservation loop upon exit
}

void LogExporter::capture_log_line(const std::string& message) noexcept {
    std::lock_guard<std::mutex> lock(m_export_mutex);
    m_buffered_logs.push_back(message);
    // Simultaneously print to stdout for interactive viewing comfort
    std::cout << message << "\n";
}

void LogExporter::execute_disk_export() noexcept {
    std::lock_guard<std::mutex> lock(m_export_mutex);
    if (m_buffered_logs.empty()) return;

    std::ofstream out_file(m_target_filename, std::ios::out | std::ios::trunc);
    if (!out_file.is_open()) {
        std::cerr << "❌ [EXPORTER ERROR]: Unresolved permission faults. Cannot write black-box logs to disk.\n";
        return;
    }

    out_file << "================================================================================\n";
    out_file << "📡 SKYWATCH-20 TACTICAL SYSTEM: MISSION AUDIT & ADVANCED HYDROKINETIC FLIGHT LOG\n";
    out_file << "================================================================================\n";
    
    for (const auto& log_entry : m_buffered_logs) {
        out_file << log_entry << "\n";
    }

    out_file << "\n================================================================================\n";
    out_file.close();
    std::cout << "✅ [EXPORTER]: Logs captured and saved to " << m_target_filename << ".\n";
}
