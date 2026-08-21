@echo off
title SKYwatch-20 Enterprise Radar Console

:: Force working directory to the directory containing this batch script
cd /d "%~dp0"

echo ========================================================
echo  Initializing SKYwatch-20 Mission Control Server...
echo ========================================================

:: Check if Python is installed
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] Python was not found in PATH. Please install Python 3.
    pause
    exit /b 1
)

:: Spawn gui_server in background
start "SKYwatch-20 Backend Server" /min python gui_server.py

:: Give server 2 seconds to initialize ports 8000 & 8080
timeout /t 2 /nobreak >nul

:: Launch default web browser
echo [*] Launching WebGL Tactical Interface...
start http://localhost:8000/index.html

echo.
echo [+] SKYwatch-20 active on http://localhost:8000
echo [*] Server is running in a minimized background window.
