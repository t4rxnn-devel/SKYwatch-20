import subprocess
import sys
import os

def build_standalone():
    print("[*] Installing PyInstaller...")
    subprocess.check_call([sys.executable, "-m", "pip", "install", "pyinstaller"])

    print("[*] Packaging gui_server.py into standalone executable...")
    cmd = [
        "pyinstaller",
        "--onefile",
        "--add-data", f"include{os.pathsep}include",
        "--add-data", f"config{os.pathsep}config",
        "--name", "skywatch_server",
        "gui_server.py"
    ]
    subprocess.check_call(cmd)
    print("[+] Standalone build complete. Binaries located in dist/")

if __name__ == "__main__":
    build_standalone()
