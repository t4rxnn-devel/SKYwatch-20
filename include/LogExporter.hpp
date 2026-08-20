// SkyWatch-20 Cryptographically Secured Log Exporter Utility Engine
// Protects exported mission records via AES-256-CBC Encryption

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

    // Cryptographic keys (Industry-standard 256-bit keys and 128-bit Initial Vectors)
    unsigned char m_aes_key[32]; 
    unsigned char m_aes_iv[16];  

    // Performs the mathematical block transformations natively
    [[nodiscard]] std::vector<unsigned char> encrypt_payload(const std::string& plaintext) const noexcept;

public:
    explicit LogExporter(std::string output_path) noexcept;
    ~LogExporter();

    void capture_log_line(const std::string& message) noexcept;
    void execute_disk_export() noexcept;
};
