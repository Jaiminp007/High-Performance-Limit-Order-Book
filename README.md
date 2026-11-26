# Quantum Trading Systems - Order Book Engine

A high-performance limit order book with an interactive trading terminal built in C++17. Features professional-grade matching engine and real-time execution reporting.

---

## 🚀 Quick Start

### Run the Trading Terminal
```bash
./start_trading.sh
```

Or launch the professional showcase:
```bash
./showcase.sh
```

### Build from Source
```bash
make              # Build everything
make terminal     # Build just the terminal
```

---

## 📦 What's Included

### Interactive Programs

| Program | Description |
|---------|-------------|
| **trading_terminal** | Interactive trading interface with real-time reporting |
| **orderbook** | Automated test suite with 9 validation scenarios |
| **showcase.sh** | Professional demo launcher with menu |
| **start_trading.sh** | Quick terminal launcher |

### Core Components

- **OrderBook Engine** - High-performance matching engine with Price-Time Priority
- **Trading Terminal** - Professional UI for interactive order management
- **Test Suite** - Comprehensive validation of all functionality

---

## 💻 Trading Terminal Features

### Interactive Menu

```
=== ACTIONS ===
1) Market Buy     2) Market Sell
3) Limit Buy      4) Limit Sell
5) Cancel Order   6) View Book
7) Run Demo       8) Clear Screen
9) Statistics     0) Exit
```

### Real-Time Execution Reports

```
+++ EXECUTION REPORT +++
Time         Trade ID  Buy ID   Sell ID  Price     Qty
-------------------------------------------------------------
12:34:56.789  1         1005     1003     10050     100
12:34:56.790  2         1005     1004     10060     20
```

### Live Order Book Display

```
========== ORDER BOOK ==========

--- ASKS (SELL ORDERS) ---
  Price: 10070 | Qty: 200 | Orders: 1
  Price: 10060 | Qty: 150 | Orders: 1
  Price: 10050 | Qty: 100 | Orders: 1

--- SPREAD ---
  Best Ask: 10050 | Best Bid: 9950 | Spread: 100

--- BIDS (BUY ORDERS) ---
  Price: 9950 | Qty: 100 | Orders: 1
  Price: 9940 | Qty: 150 | Orders: 1
  Price: 9930 | Qty: 200 | Orders: 1
```

### Session Statistics

Track your trading activity:
- Total orders placed
- Number of trades executed
- Total volume traded
- Timestamp tracking

---

## 🎯 Usage Examples

### Example 1: Run Demo Mode
```bash
./trading_terminal
Choice: 7
```
This runs an automated simulation showing the matching engine in action.

### Example 2: Place Limit Order
```bash
Choice: 3
Price: 10000
Quantity: 100
[12:34:56.789] Limit order posted (ID: 1000)
```

### Example 3: Execute Market Order
```bash
Choice: 1
Quantity: 50

+++ EXECUTION REPORT +++
Time         Trade ID  Buy ID   Sell ID  Price     Qty
-------------------------------------------------------------
12:34:56.789  1         1002     1000     10000     50
```

Market orders are immediate-or-cancel: unfilled quantity is cancelled instead of resting in the book.

### Example 4: Cancel Order
```bash
Choice: 5
Order ID to cancel: 1000
[12:34:56.789] Cancelled order 1000
```

---

## ⚡ Performance

### Time Complexity

| Operation | Complexity | Details |
|-----------|-----------|---------|
| Add Order | O(log n + m) | n = price levels, m = matches |
| Cancel Order | O(1) average | Hash lookup + list erase |
| Match Order | O(m) | m = number of orders matched |

### Throughput Estimates

These are design estimates, not benchmarked guarantees:

- **10M** orders/sec (no matching)
- **2-5M** orders/sec (with matching)
- **15M** cancellations/sec

### Memory

- ~104 bytes per active order
- No pre-allocation
- Scales linearly

---

## 🏗️ Architecture

### Data Structures

**Price Levels:** `std::map` - Maintains sorted order automatically
- Bids sorted descending (highest first)
- Asks sorted ascending (lowest first)

**Orders at Each Level:** `std::list` - FIFO queue
- O(1) removal from any position
- Iterator stability for lookups

**Order Lookup:** `std::unordered_map` - Fast cancellation
- O(1) average lookup time
- Stores iterator for direct access

### Matching Engine

Implements **Price-Time Priority**:
1. Orders match first by price
2. At the same price, FIFO order (first in, first out)
3. Partial fills supported
4. Unfilled limit-order quantity rests in the book
5. Unfilled market-order quantity is cancelled

---

## 📚 Documentation

- **TRADING_GUIDE.md** - User manual for the terminal
- **TERMINAL_PACKAGE.md** - Complete package overview
- **DESIGN.md** - Technical architecture and algorithms
- **QUICKSTART.md** - Quick reference guide

---

## 🔧 Build System

### Available Targets

```bash
make                # Build all components
make terminal       # Build just the trading terminal
make orderbook      # Build just the test suite
make run            # Build and run test suite
make test           # Build and run correctness tests
make run-terminal   # Build and run trading terminal
make clean          # Remove all binaries
make rebuild        # Clean and rebuild everything
make debug          # Build with debug symbols
make performance    # Build with max optimization
```

### Requirements

- C++17 compatible compiler (g++, clang++)
- Standard library only (no external dependencies)
- Unix-like environment (Linux, macOS)

---

## 📁 Project Structure

```
c++ project/
├── TradingTerminal.cpp    # Interactive trading UI
├── OrderBook.h            # API definitions
├── OrderBook.cpp          # Matching engine implementation
├── main.cpp               # Automated correctness tests and scenarios
├── Makefile               # Build system
├── start_trading.sh       # Terminal launcher
├── showcase.sh            # Demo launcher
├── verify.sh              # Verification script
├── TRADING_GUIDE.md       # Terminal user manual
├── TERMINAL_PACKAGE.md    # Complete package overview
├── DESIGN.md              # Technical architecture
└── QUICKSTART.md          # Quick reference
```

---

## 🎓 Key Features

### Matching Engine

✅ **Price-Time Priority** - Fair matching based on price then time  
✅ **Immediate Matching** - Aggressive orders match instantly  
✅ **Partial Fills** - Orders can be partially executed  
✅ **Multi-Level Matching** - Orders can consume multiple price levels  
✅ **Automatic Cleanup** - Empty price levels removed automatically  

### Trading Terminal

✅ **Real-Time Reporting** - Microsecond precision timestamps  
✅ **Session Tracking** - Monitor orders, trades, and volume  
✅ **Multiple Order Types** - Immediate-or-cancel market and resting limit orders supported
✅ **Order Management** - Cancel orders by ID  
✅ **Demo Mode** - Automated simulation included  
✅ **Professional UI** - Clean, intuitive interface  

### Code Quality

✅ **Zero Warnings** - Compiles cleanly with -Wall -Wextra -pedantic  
✅ **Modern C++** - Uses C++17 features and best practices  
✅ **No Dependencies** - Standard library only  
✅ **Well Documented** - Comprehensive inline and external docs  
✅ **Validated Inputs** - Rejects zero quantity, zero price, and duplicate active order IDs

---

## 🎯 Use Cases

### Educational
- Learn how limit order books work
- Study matching engine algorithms
- Practice with C++17 features
- Understand financial systems

### Professional
- Portfolio projects
- Technical interviews
- Trading system prototypes
- Algorithm research

### Development
- Test trading strategies
- Benchmark performance
- Prototype new features
- Market simulation

---

## 🔍 Testing

### Run Automated Tests
```bash
./orderbook
```

This runs explicit correctness checks plus 9 demonstration scenarios:
1. Initial book building
2. Aggressive sell order (multi-level match)
3. Aggressive buy order (partial fill)
4. Adding resting orders
5. Order cancellation
6. Market sweep
7. Invalid cancellation handling
8. Immediate complete matching
9. Performance summary

### Verify Installation
```bash
./verify.sh
```

Runs compilation tests and validates all functionality.

---

## 🚀 Getting Started Guide

### Step 1: Build Everything
```bash
make
```

### Step 2: Run the Demo
```bash
./showcase.sh
```
Select option 1 to launch the trading terminal.

### Step 3: Try Demo Mode
Once in the terminal:
```
Choice: 7
```
This runs an automated simulation.

### Step 4: Try Manual Trading
```
Choice: 3          # Limit Buy
Price: 10000
Quantity: 100

Choice: 4          # Limit Sell
Price: 10100
Quantity: 50

Choice: 6          # View Book
```

### Step 5: Read the Docs
```bash
cat TRADING_GUIDE.md
```

---

## 💡 Tips

- **Start with demo mode** (option 7) to understand how it works
- **View the book often** (option 6) to see current state
- **Note order IDs** when placing orders for cancellation
- **Use statistics** (option 9) to track your session
- **Clear screen** (option 8) to refresh the display

---

## 🐛 Troubleshooting

### Build Fails
```bash
make clean
make
```

### Terminal Won't Start
```bash
make terminal
chmod +x start_trading.sh
./start_trading.sh
```

### Orders Not Matching
- Check that prices cross the spread
- Buy price must be >= best ask
- Sell price must be <= best bid
- View book (option 6) to see current prices

---

## 🔒 Code Quality Verification

### Compile with Strict Warnings
```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror TradingTerminal.cpp OrderBook.cpp -o trading_terminal
```
Result: Zero warnings, zero errors ✅

### Run Verification Suite
```bash
./verify.sh
```
All checks pass ✅

---

## 📈 Performance Benchmarks

Estimated performance on a modern CPU (not a benchmarked guarantee):

**Order Operations:**
- Add order (no match): ~50-100 ns
- Add order (with match): ~100-500 ns
- Cancel order: ~30-80 ns

**Throughput:**
- 10M order additions/sec (no match)
- 2-5M order additions/sec (with match)
- 15M cancellations/sec

**Memory:**
- 104 bytes per active order
- Linear scaling with order count
- No leaks detected in sanitizer verification

---

## 🎨 What Makes This Different

### Professional Quality
- Production-ready code structure
- Comprehensive error handling
- Real-time reporting with precise timestamps
- Professional branding and UI

### Educational Value
- Well-documented architecture
- Clear separation of concerns
- Modern C++ best practices
- Realistic implementation patterns

### Practical Design
- No over-engineering
- Standard library only
- Efficient data structures
- Clean, readable code

---

## 🤝 Contributing

This is an educational project demonstrating order book implementation. Feel free to:
- Study the code
- Extend functionality
- Use in your projects
- Share and learn

---

## 📄 License

Educational and demonstration purposes. Free to use, modify, and learn from.

---

## 🎉 Summary

You have a complete, professional-grade trading system featuring:

✅ High-performance matching engine  
✅ Interactive trading terminal  
✅ Real-time execution reporting  
✅ Comprehensive test suite  
✅ Complete documentation  
✅ Zero external dependencies  
✅ Sanitizer-checked core matching paths

**Start trading now:**
```bash
./start_trading.sh
```

---

*Quantum Trading Systems - Order Book Engine v2.1*  
*Built with C++17 - Professional Grade Implementation*  
*October 2025*
