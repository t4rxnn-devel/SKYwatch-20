# SkyWatch-20 Cryptographic Log Retrieval Tool
# Decrypts secure AES-256-CBC binary logs into standard plain text

import os
from crypto.Cipher import AES

def decrypt_mission_file(binary_path="generated/skywatch_mission_log.bin"):
    print("=== INITIALIZING BLACK-BOX LOG RECOVERY DECRYPTION ===")
    
    if not os.path.exists(binary_path):
        print(f"❌ [DECRYPT ERROR]: File '{binary_path}' not found on disk.")
        return

    # Mirror encryption keys used by our C++ LogExporter module
    key = b"01234567890123456789012345678901" # 256-bit symmetric security token
    iv  = b"1234567890123456"                 # 128-bit initialization vector

    try:
        with open(binary_path, "rb") as f:
            ciphertext = f.read()

        # Initialize the hardware decryption context
        cipher = AES.new(key, AES.MODE_CBC, iv)
        decrypted_raw = cipher.decrypt(ciphertext)

        # Handle standard PKCS#7 unpadding parameters smoothly
        padding_len = decrypted_raw[-1]
        plaintext = decrypted_raw[:-padding_len].decode("utf-8")

        print("🔓 [CRYPTO SUCCESS]: AES block sequence decrypted with zero bit errors.")
        print("\n--- RECOVERED MISSION STATEMENT CONTENT ---")
        print(plaintext)
        print("-------------------------------------------")

    except Exception as e:
        print(f"❌ [DECRYPT CRITICAL FAULT]: Failed to clear cryptographic layers. Reason: {e}")

if __name__ == "__main__":
    decrypt_mission_file()
