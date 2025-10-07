#!/bin/bash

# Professional showcase script for the trading terminal

clear

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
read -p "Select option: " choice

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
        echo "Compiler:      $(g++ --version | head -n1)"
        echo "Build System:  GNU Make"
        echo "Dependencies:  Standard Library Only"
        echo ""
        echo "Components:"
        ls -lh orderbook trading_terminal 2>/dev/null | tail -2 | awk '{print "  " $9 " - " $5}'
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
read -p "Press Enter to continue..."
