// SkyWatch-20 High-Integrity Log Exporter Utility Engine
// Handles automated stream writing to preserve real-time system performance logs

#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>

class LogExporter {
private:
    std::string m_target_filename;
    std::vector<std::string> m_buffered_logs;
    mutable std::mutex m_export_mutex;

public:
    explicit LogExporter(std::string output_path) noexcept;
    ~LogExporter();

    // Appends telemetry data safely to the runtime memory array matrix
    void capture_log_line(const std::string& message) noexcept;

    // Executes the automated physical file output export write sequence to disk
    void execute_disk_export() noexcept;
};
