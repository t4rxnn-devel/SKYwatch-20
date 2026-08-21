@echo off
title SKYwatch-20 Radar Console
echo Starting SKYwatch-20 Engine and Server...

:: Start the Python GUI Server in background
start /b python gui_server.py

:: Give server 1 second to bind port 8000
timeout /t 2 /nobreak >nul

:: Launch browser automatically
start http://localhost:8000/index.html

echo SKYwatch-20 running on http://localhost:8000
