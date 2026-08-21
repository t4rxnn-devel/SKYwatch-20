#!/usr/bin/env bash
CD_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$CD_DIR"

echo "Launching SKYwatch-20 Console..."

# Run background server
python3 gui_server.py &
SERVER_PID=$!

sleep 2

# Open local web console depending on OS
if [[ "$OSTYPE" == "darwin"* ]]; then
    open "http://localhost:8000/index.html"
else
    xdg-open "http://localhost:8000/index.html" 2>/dev/null || echo "Please open http://localhost:8000/index.html in your browser."
fi

wait $SERVER_PID
