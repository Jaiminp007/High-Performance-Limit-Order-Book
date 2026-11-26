#!/usr/bin/env bash
set -euo pipefail

# Professional showcase script for the trading terminal

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

clear 2>/dev/null || true

cat << "EOF"
╔══════════════════════════════════════════════════════════════════╗
║                                                                  ║
║          QUANTUM TRADING SYSTEMS - ORDER BOOK ENGINE             ║
║                                                                  ║
║                    Professional Demo v2.1                        ║
║                                                                  ║
╚══════════════════════════════════════════════════════════════════╝

EOF

echo "This package includes:"
echo ""
echo "  📊 High-Performance Order Book Engine"
echo "  💻 Interactive Trading Terminal"
echo "  📈 Real-Time Execution Reporting"
echo "  📉 Live Market Depth Display"
echo ""
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""
echo "Available Programs:"
echo ""
echo "  1) Trading Terminal    - Interactive trading interface"
echo "  2) Test Suite         - Automated test scenarios"
echo "  3) Documentation      - View user guides"
echo "  4) Build Info         - Show build information"
echo "  0) Exit"
echo ""
read -p "Select option: " choice || exit 0

case $choice in
    1)
        echo ""
        echo "Launching Trading Terminal..."
        sleep 1
        ./start_trading.sh
        ;;
    2)
        echo ""
        echo "Running Test Suite..."
        sleep 1
        if [[ ! -x ./orderbook ]]; then
            make orderbook
        fi
        ./orderbook
        ;;
    3)
        echo ""
        echo "Opening Documentation..."
        sleep 0.5
        if command -v less &> /dev/null; then
            less TRADING_GUIDE.md
        else
            cat TRADING_GUIDE.md
        fi
        ;;
    4)
        echo ""
        echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        echo "BUILD INFORMATION"
        echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        echo ""
        echo "Language:      C++17"
        if command -v g++ >/dev/null 2>&1; then
            echo "Compiler:      $(g++ --version | head -n1)"
        else
            echo "Compiler:      g++ not found"
        fi
        echo "Build System:  GNU Make"
        echo "Dependencies:  Standard Library Only"
        echo ""
        echo "Components:"
        for component in orderbook trading_terminal; do
            if [[ -x "$component" ]]; then
                ls -lh "$component" | awk '{print "  " $9 " - " $5}'
            else
                echo "  $component - not built"
            fi
        done
        echo ""
        echo "Performance:"
        echo "  • Order Processing: O(log n + m)"
        echo "  • Cancellation: O(1) average"
        echo "  • Matching: O(m) where m = matches"
        echo ""
        echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
        ;;
    0)
        echo ""
        echo "Thank you for using Quantum Trading Systems."
        echo ""
        exit 0
        ;;
    *)
        echo ""
        echo "Invalid option."
        ;;
esac

echo ""
read -p "Press Enter to continue..." || true
