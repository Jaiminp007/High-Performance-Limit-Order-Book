# Quick Start Guide

## Project Overview

High-performance limit order book implementation for high-frequency trading in C++17.

## Files

| File | Description |
|------|-------------|
| `OrderBook.h` | Header with class definitions and API |
| `OrderBook.cpp` | Implementation of matching engine |
| `main.cpp` | Comprehensive test driver |
| `Makefile` | Build automation |
| `README.md` | Complete project documentation |
| `DESIGN.md` | Technical design document |

## Quick Start

### 1. Build the Project

```bash
# Using Makefile
make

# Or manually
g++ -std=c++17 -Wall -Wextra -O2 -o orderbook main.cpp OrderBook.cpp
```

### 2. Run the Demo

```bash
./orderbook
```

### 3. Use in Your Code

```cpp
#include "OrderBook.h"

int main() {
    OrderBook book;
    
    // Add orders
    auto trades = book.AddOrder(1, Side::BUY, 100, 50);
    
    // Process trades
    for (const auto& trade : trades) {
        // Handle trade...
    }
    
    // Cancel order
    book.CancelOrder(1);
    
    // View book
    book.PrintOrderBook();
    
    return 0;
}
```

## Key Features

✅ **Price-Time Priority** - Fair matching based on price then time  
✅ **O(1) Cancellation** - Fast order cancellation by ID  
✅ **O(log n) Insertion** - Efficient order addition  
✅ **FIFO at Price Level** - Orders at same price processed first-in-first-out  
✅ **Immediate Matching** - Aggressive orders match instantly  
✅ **No Dependencies** - Only C++17 Standard Library  

## API Reference

### AddOrder()

```cpp
std::vector<Trade> AddOrder(uint64_t order_id, Side side, 
                             uint64_t price, uint64_t quantity);
```

**Parameters:**
- `order_id`: Unique identifier for the order
- `side`: `Side::BUY` or `Side::SELL`
- `price`: Order price (uint64_t)
- `quantity`: Order quantity (uint64_t)

**Returns:** Vector of trades generated from matching

**Behavior:**
1. Validates positive price and quantity
2. Rejects duplicate active order IDs by throwing `std::invalid_argument`
3. Attempts to match immediately
4. Returns trades executed
5. Adds remaining quantity as a resting limit order

### AddMarketOrder()

```cpp
std::vector<Trade> AddMarketOrder(uint64_t order_id, Side side, uint64_t quantity);
```

**Behavior:**
1. Validates positive quantity
2. Rejects duplicate active order IDs by throwing `std::invalid_argument`
3. Matches immediately against available resting liquidity
4. Cancels unfilled quantity instead of resting it

### CancelOrder()

```cpp
bool CancelOrder(uint64_t order_id);
```

**Parameters:**
- `order_id`: ID of order to cancel

**Returns:** `true` if the order existed and was cancelled, otherwise `false`

**Behavior:**
- Removes order from book if found
- Returns `false` if order doesn't exist

### PrintOrderBook()

```cpp
void PrintOrderBook() const;
```

**Behavior:**
- Prints all asks (highest to lowest)
- Prints spread information
- Prints all bids (highest to lowest)
- Shows quantity and order count per level

## Example Scenarios

### Scenario 1: Build a Book

```cpp
OrderBook book;

// Add resting orders
book.AddOrder(1, Side::BUY, 100, 50);   // Bid
book.AddOrder(2, Side::SELL, 101, 30);  // Ask
book.AddOrder(3, Side::BUY, 99, 40);    // Lower bid
book.AddOrder(4, Side::SELL, 102, 25);  // Higher ask

book.PrintOrderBook();
```

### Scenario 2: Match Orders

```cpp
OrderBook book;

// Setup
book.AddOrder(1, Side::SELL, 100, 50);

// Aggressive buy crosses spread
auto trades = book.AddOrder(2, Side::BUY, 100, 30);

// Process trades
for (const auto& t : trades) {
    std::cout << "Executed: " << t.quantity << " @ " << t.price << "\n";
}
```

### Scenario 3: Cancel Orders

```cpp
OrderBook book;

book.AddOrder(1, Side::BUY, 100, 50);
book.PrintOrderBook();

// Cancel it
bool cancelled = book.CancelOrder(1);
book.PrintOrderBook();  // Now empty
```

## Make Targets

```bash
make              # Build release version
make debug        # Build with debug symbols
make performance  # Build with aggressive optimizations
make run          # Build and run
make test         # Build and run correctness tests
make clean        # Remove build artifacts
make rebuild      # Clean and rebuild
make help         # Show all targets
```

## Performance Tips

### For Maximum Speed:

1. **Use performance build:**
   ```bash
   make performance
   ```

2. **Minimize allocations:**
   - Reuse Trade vectors
   - Pre-size containers if order count known

3. **Batch operations:**
   - Group order additions
   - Process trades in bulk

4. **Profile hot paths:**
   ```bash
   g++ -std=c++17 -O2 -pg -o orderbook main.cpp OrderBook.cpp
   ./orderbook
   gprof orderbook gmon.out > profile.txt
   ```

## Common Pitfalls

❌ **DON'T:**
- Reuse active order IDs (the API rejects them)
- Add zero quantity or zero price orders (the API rejects them)
- Modify Order objects after adding to book

✅ **DO:**
- Generate unique order IDs
- Validate inputs before calling API and handle `std::invalid_argument`
- Check Trade vector size before accessing

## Debugging

### Enable Debug Build

```bash
make debug
lldb ./orderbook_debug
```

### Add Logging

```cpp
// In OrderBook.cpp
void OrderBook::AddOrder(...) {
    std::cerr << "Adding order " << order_id << "\n";
    // ... rest of implementation
}
```

### Visualize State

```cpp
// After each operation
book.PrintOrderBook();
```

## Testing

The `main.cpp` demonstrates:
- ✅ Building initial book
- ✅ Aggressive orders
- ✅ Partial fills
- ✅ Order cancellation
- ✅ Market sweeps
- ✅ Edge cases

## Extending the Code

### Add Order Types

```cpp
enum class OrderType { LIMIT, MARKET, IOC, FOK };
```

### Add Callbacks

```cpp
using TradeCallback = std::function<void(const Trade&)>;

class OrderBook {
    void SetTradeCallback(TradeCallback cb) { 
        trade_callback_ = cb; 
    }
private:
    TradeCallback trade_callback_;
};
```

### Add Validation

```cpp
bool OrderBook::ValidateOrder(uint64_t id, uint64_t price, uint64_t qty) {
    if (order_lookup_.count(id)) return false;  // Duplicate ID
    if (price == 0) return false;
    if (qty == 0) return false;
    return true;
}
```

## Benchmarking

### Simple Benchmark

```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();

for (int i = 0; i < 1000000; i++) {
    book.AddOrder(i, Side::BUY, 100, 10);
}

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

std::cout << "Time: " << duration.count() << " μs\n";
std::cout << "Throughput: " << (1000000.0 / duration.count()) << " M ops/sec\n";
```

## Resources

- **Full Documentation**: `README.md`
- **Design Details**: `DESIGN.md`
- **Source Code**: `OrderBook.h`, `OrderBook.cpp`
- **Test Examples**: `main.cpp`

## Support

For issues or questions:
1. Check the design document
2. Review the test cases
3. Add debug output
4. Examine with debugger

## License

Educational example - use freely for learning and development.

---

**Happy Trading! 🚀📈**
