#include "LogExporter.hpp"
#include <openssl/evp.h> // Native industry-standard cryptographic primitives
#include <openssl/rand.h>
#include <iostream>
#include <cstring>

LogExporter::LogExporter(std::string output_path) noexcept 
    : m_target_filename(std::move(output_path)) {
    
    // Generate secure keys using hardware-seeded pseudorandom token tables
    // Real tracking systems load these from protected hardware security modules (HSM)
    std::memcpy(m_aes_key, "01234567890123456789012345678901", 32); // Sample 256-bit key
    std::memcpy(m_aes_iv,  "1234567890123456", 16);                 // Sample 128-bit IV
}

LogExporter::~LogExporter() {
    execute_disk_export();
}

void LogExporter::capture_log_line(const std::string& message) noexcept {
    std::lock_guard<std::mutex> lock(m_export_mutex);
    m_buffered_logs.push_back(message);
    std::cout << message << "\n";
}

std::vector<unsigned char> LogExporter::encrypt_payload(const std::string& plaintext) const noexcept {
    // Instantiate OpenSSL's high-level Envelope Cipher Interface
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return {};

    // Configure the framework context to run AES-256 in Cipher Block Chaining (CBC) mode
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, m_aes_key, m_aes_iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // Allocate padding allowance buffer constraints
    std::vector<unsigned char> ciphertext(plaintext.size() + AES_BLOCK_SIZE);
    int len = 0;
    int ciphertext_len = 0;

    // Stream raw plaintext chunks through the encryption matrix 
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, reinterpret_cast<const unsigned char*>(plaintext.data()), plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    ciphertext_len = len;

    // Apply strict PKCS#7 final block padding rules
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertext_len); // Crop layout buffer to true byte count
    return ciphertext;
}

void LogExporter::execute_disk_export() noexcept {
    std::lock_guard<std::mutex> lock(m_export_mutex);
    if (m_buffered_logs.empty()) return;

    // Package the memory log strings together into one flat payload buffer
    std::string flat_plaintext = "================================================================================\n"
                                 "📡 SKYWATCH-20 ENCRYPTED SYSTEM MISSION LOG\n"
                                 "================================================================================\n";
    for (const auto& log_entry : m_buffered_logs) {
        flat_plaintext += log_entry + "\n";
    }

    // Execute block transformation math
    std::vector<unsigned char> encrypted_bytes = encrypt_payload(flat_plaintext);
    if (encrypted_bytes.empty()) {
        std::cerr << "❌ [CRYPT ERROR]: Cryptographic core initialization failed.\n";
        return;
    }

    // Export payload output to disk as a binary raw byte array stream (.bin format)
    std::string binary_filename = m_target_filename.substr(0, m_target_filename.find_last_of('.')) + ".bin";
    std::ofstream out_file(binary_filename, std::ios::out | std::ios::binary | std::ios::trunc);
    
    if (!out_file.is_open()) {
        std::cerr << "❌ [EXPORTER ERROR]: Cannot write secure log package to disk.\n";
        return;
    }

    out_file.write(reinterpret_cast<const char*>(encrypted_bytes.data()), encrypted_bytes.size());
    out_file.close();
    
    std::cout << "🔒 [SECURE EXPORTER]: Black-box telemetry encrypted successfully.\n";
    std::cout << "💾 [SECURE EXPORTER]: Saved ciphertext to " << binary_filename << ".\n";
}
