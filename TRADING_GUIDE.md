# Quantum Trading Terminal - User Guide

A professional real-time order book trading interface built with C++17.

## Quick Start

### Launch Terminal
```bash
./start_trading.sh
```

Or directly:
```bash
./trading_terminal
```

## Features

✨ **Interactive Trading** - Menu-driven interface  
✨ **Real-Time Execution** - Microsecond timestamps  
✨ **Session Statistics** - Track orders, trades, volume  
✨ **Multiple Order Types** - Market and limit orders  
✨ **Order Management** - Cancel by ID  
✨ **Live Book Display** - View market depth  
✨ **Demo Mode** - Automated simulation  

## Menu Options

| Option | Action | Description |
|--------|--------|-------------|
| 1 | Market Buy | Execute immediate buy |
| 2 | Market Sell | Execute immediate sell |
| 3 | Limit Buy | Place buy order at price |
| 4 | Limit Sell | Place sell order at price |
| 5 | Cancel Order | Remove order by ID |
| 6 | View Book | Display order book |
| 7 | Run Demo | Automated simulation |
| 8 | Clear Screen | Refresh display |
| 9 | Statistics | Show session stats |
| 0 | Exit | Close terminal |

## Order Types

### Market Orders
Execute immediately at best available price. May result in partial fills.

**Example:**
```
Choice: 1
Quantity: 50
Max Price: 10100
```

### Limit Orders
Rest in book at specified price. Execute only at limit price or better.

**Example:**
```
Choice: 3
Price: 10000
Quantity: 100
[12:34:56.789] Limit order posted (ID: 1000)
```

## Demo Mode

Run option 7 to see an automated trading scenario:
1. Initializes market with liquidity
2. Displays order book
3. Executes aggressive order
4. Shows trades and updated book

## Tips

💡 **Start with Demo** (option 7) to understand the system  
💡 **View Book Often** (option 6) to see market state  
💡 **Note Order IDs** for later cancellation  
💡 **Use Statistics** (option 9) to track performance  

## Troubleshooting

**Build Issues?**
```bash
make clean && make terminal
```

**Orders Not Matching?**
- Check prices cross the spread
- View book to see current prices
- Verify quantity is sufficient

## Technical Details

- **Language**: C++17
- **Performance**: Sub-millisecond processing
- **Dependencies**: Standard library only
- **Order ID Range**: Starting from 1000

## Build Commands

```bash
make terminal          # Build terminal
make run-terminal      # Build and run
make clean            # Remove binaries
```

---

**Happy Trading!** 📈💰
