#!/usr/bin/env bash
set -euo pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

echo "================================================="
echo "  ORDER BOOK PROJECT VERIFICATION"
echo "================================================="
echo ""

workdir="$(mktemp -d)"
trap 'rm -rf "$workdir"' EXIT

echo "[1/5] Compiling engine and test suite with strict warnings..."
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2 -c OrderBook.cpp -o "$workdir/OrderBook.o"
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2 -c main.cpp -o "$workdir/main.o"
echo "OrderBook.cpp and main.cpp compile with -Werror"

echo ""
echo "[2/5] Compiling trading terminal with strict warnings..."
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2 \
    TradingTerminal.cpp OrderBook.cpp -o "$workdir/trading_terminal_verify"
echo "TradingTerminal.cpp compiles with -Werror"

echo ""
echo "[3/5] Linking automated test suite..."
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -O2 \
    -o "$workdir/orderbook_verify" "$workdir/OrderBook.o" "$workdir/main.o"
echo "Linking successful"

echo ""
echo "[4/5] Running automated correctness tests..."
"$workdir/orderbook_verify" > /dev/null
echo "Automated correctness tests pass"

echo ""
echo "[5/5] Checking terminal startup and project structure..."
printf '0\n' | "$workdir/trading_terminal_verify" > /dev/null
required_files=("OrderBook.h" "OrderBook.cpp" "main.cpp" "TradingTerminal.cpp" "Makefile" "README.md")
for file in "${required_files[@]}"; do
    if [[ ! -f "$file" ]]; then
        echo "Missing required file: $file"
        exit 1
    fi
done
echo "Terminal startup and project structure pass"

echo ""
echo "================================================="
echo "  ALL VERIFICATION CHECKS PASSED"
echo "================================================="
echo ""
echo "Quick Commands:"
echo "  make              # Build release version"
echo "  make run          # Build and run automated tests"
echo "  make terminal     # Build interactive terminal"
echo "  ./verify.sh       # Run this verification suite"
echo ""
