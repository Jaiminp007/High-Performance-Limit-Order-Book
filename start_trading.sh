#!/usr/bin/env bash
set -euo pipefail

# Trading Terminal Launcher
# Professional order book trading interface

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

clear 2>/dev/null || true

echo "╔══════════════════════════════════════════════════════════╗"
echo "║                                                          ║"
echo "║     QUANTUM TRADING SYSTEMS                              ║"
echo "║     Order Book Terminal v2.1                             ║"
echo "║                                                          ║"
echo "╚══════════════════════════════════════════════════════════╝"
echo ""
echo "Initializing trading environment..."
sleep 0.5

# Check if terminal exists
if [ ! -f "./trading_terminal" ]; then
    echo "Building trading terminal..."
    make terminal
    echo ""
fi

echo "Connecting to matching engine..."
sleep 0.3
echo "Loading market data..."
sleep 0.3
echo "Starting terminal..."
echo ""
sleep 0.5

./trading_terminal
