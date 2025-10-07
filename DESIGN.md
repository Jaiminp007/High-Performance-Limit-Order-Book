# Technical Design Document: High-Performance Limit Order Book

## 1. System Overview

This document describes the architecture and design decisions for a high-performance, single-threaded limit order book implementation suitable for high-frequency trading (HFT) applications.

### 1.1 Goals

- **Ultra-low latency**: Minimize time to process orders and execute trades
- **Correctness**: Strict Price-Time Priority matching
- **Simplicity**: Single-threaded, no external dependencies
- **Maintainability**: Clean, well-documented code

### 1.2 Non-Goals

- Multi-threading/concurrent access
- Persistent storage
- Network communication
- Complex order types (IOC, FOK, etc.)

---

## 2. Architecture

### 2.1 Component Diagram

```
┌─────────────────────────────────────────┐
│           OrderBook Class               │
├─────────────────────────────────────────┤
│  Public API:                            │
│  - AddOrder()                           │
│  - CancelOrder()                        │
│  - PrintOrderBook()                     │
├─────────────────────────────────────────┤
│  Private Data:                          │
│  - bids_ (sorted descending)            │
│  - asks_ (sorted ascending)             │
│  - order_lookup_ (hash map)             │
│  - next_trade_id_                       │
├─────────────────────────────────────────┤
│  Private Methods:                       │
│  - MatchBuyOrder()                      │
│  - MatchSellOrder()                     │
│  - AddRestingOrder()                    │
│  - RemoveOrder()                        │
└─────────────────────────────────────────┘
```

### 2.2 Data Structure Design

#### Bid/Ask Storage

```cpp
// Bids: Higher prices first
std::map<uint64_t, std::list<Order>, std::greater<uint64_t>> bids_;

// Asks: Lower prices first  
std::map<uint64_t, std::list<Order>> asks_;
```

**Rationale:**
- `std::map` provides O(log n) insertion and maintains sorted order
- `std::greater<>` comparator for bids ensures descending price order
- `std::list` at each price level ensures FIFO and O(1) removal

#### Order Lookup

```cpp
struct OrderLocation {
    uint64_t price;
    std::list<Order>::iterator iterator;
};

std::unordered_map<uint64_t, OrderLocation> order_lookup_;
```

**Rationale:**
- O(1) average case lookup for cancellation
- Stores iterator for direct order removal without search
- Minimal memory overhead per order

---

## 3. Algorithmic Analysis

### 3.1 Order Addition

**Time Complexity: O(log n + m)**

Where:
- n = number of price levels
- m = number of orders matched

**Algorithm:**
1. Create Order object: O(1)
2. Match against opposite side:
   - Find best price level: O(log n)
   - Match orders at level: O(m) where m is matches
   - Remove empty price levels: O(1) per level
3. Add remaining quantity as resting order:
   - Insert into map: O(log n)
   - Append to list: O(1)
   - Update lookup table: O(1)

**Hot Path Optimization:**
- Early exit when price doesn't cross spread
- Minimize allocations during matching
- Reuse Trade vector capacity

### 3.2 Order Cancellation

**Time Complexity: O(1) average**

**Algorithm:**
1. Lookup order location: O(1) average (hash table)
2. Erase from list: O(1)
3. Remove price level if empty: O(1)
4. Remove from lookup table: O(1)

**Key Optimization:**
- Store iterator directly, avoiding linear search
- Single allocation/deallocation per cancel

### 3.3 Matching Engine

**Price-Time Priority Implementation:**

```
For BUY order at price P:
    While quantity remains:
        Get best ask (lowest price A)
        If P < A: break (no match)
        
        While quantity remains and orders at A exist:
            Match with first order (FIFO)
            Generate trade
            Update quantities
            Remove filled orders
```

**Critical Path:**
- No dynamic allocation during matching
- In-place quantity updates
- Efficient iterator-based traversal

---

## 4. Memory Management

### 4.1 Memory Layout

**Per Order Storage:**
- Order structure: ~32 bytes
- List node overhead: ~16 bytes
- Map node overhead: ~32 bytes
- Lookup entry: ~24 bytes
- **Total per order: ~104 bytes**

### 4.2 Memory Efficiency

**Current Design:**
- No pre-allocation
- Memory scales linearly with active orders
- Automatic cleanup of filled orders

**Potential Optimizations:**
- Object pools for Order structures
- Custom allocators for list nodes
- Memory arena for price levels

---

## 5. Performance Characteristics

### 5.1 Expected Latency (Modern CPU)

| Operation | Typical Latency | Notes |
|-----------|----------------|-------|
| Add Order (no match) | 50-100 ns | Map insert + list append |
| Add Order (with match) | 100-500 ns | Depends on matches |
| Cancel Order | 30-80 ns | Hash lookup + list erase |
| Print Book | 1-10 μs | Scales with order count |

### 5.2 Throughput Estimates

**Single-threaded on modern CPU:**
- ~10M order additions/sec (no matching)
- ~2-5M order additions/sec (with matching)
- ~15M cancellations/sec

**Bottlenecks:**
1. Map operations (log n) for new price levels
2. Iterator traversal during matching
3. Memory allocation/deallocation

---

## 6. Design Decisions

### 6.1 Why std::map?

**Alternatives Considered:**
- `std::unordered_map`: No ordering guarantee
- Array/vector: Fixed size, wasteful for sparse prices
- Skip list: More complex, similar performance

**Decision:** `std::map`
- Built-in sorting
- Proven performance
- Standard library reliability

### 6.2 Why std::list?

**Alternatives Considered:**
- `std::deque`: Better cache locality
- `std::vector`: Expensive removal from middle

**Decision:** `std::list`
- O(1) removal from any position
- Iterator stability (critical for lookup map)
- True FIFO semantics

### 6.3 Trade-offs

| Aspect | Current Design | Alternative |
|--------|---------------|------------|
| Price levels | std::map (sorted) | Fixed array (faster) |
| Orders per level | std::list (flexible) | std::deque (cache-friendly) |
| Order lookup | std::unordered_map | No fast cancellation |
| Thread safety | None (single-threaded) | Lock-based or lock-free |

---

## 7. Testing Strategy

### 7.1 Unit Test Coverage

**Critical Scenarios:**
1. ✅ Order addition without matching
2. ✅ Partial order matching
3. ✅ Full order matching
4. ✅ Multi-level matching
5. ✅ Order cancellation
6. ✅ Invalid cancellation handling
7. ✅ Price-Time priority verification
8. ✅ FIFO ordering at price level

### 7.2 Edge Cases

- Empty book operations
- Single-sided book
- Price level exhaustion
- Large order sweeping multiple levels
- Duplicate order IDs (not tested, should add)
- Zero quantity orders (not tested, should add)

---

## 8. Production Considerations

### 8.1 Required Enhancements

**For Production Use:**

1. **Input Validation**
   - Order ID uniqueness
   - Quantity > 0
   - Price within valid range
   - Side validation

2. **Error Handling**
   - Return codes/exceptions for invalid operations
   - Logging infrastructure
   - Metrics/monitoring

3. **Performance Monitoring**
   - Latency histograms
   - Throughput counters
   - Memory usage tracking

4. **Concurrency**
   - Lock-free data structures
   - Reader-writer locks
   - Thread-per-symbol model

### 8.2 Advanced Features

**Next Generation:**

1. **Order Types**
   - Immediate-or-Cancel (IOC)
   - Fill-or-Kill (FOK)
   - Stop/Stop-Limit orders
   - Iceberg orders

2. **Market Data**
   - Level 2 market depth
   - Incremental updates
   - Snapshot generation

3. **Risk Management**
   - Position limits
   - Price bands
   - Circuit breakers

---

## 9. Benchmarking

### 9.1 Test Methodology

```cpp
// Pseudo-code for benchmark
for (int i = 0; i < 1000000; i++) {
    auto start = high_resolution_clock::now();
    book.AddOrder(id++, Side::BUY, price, qty);
    auto end = high_resolution_clock::now();
    latencies.push_back(duration_cast<nanoseconds>(end - start));
}

// Report percentiles: p50, p95, p99, p99.9
```

### 9.2 Expected Results (Modern Intel/AMD CPU)

**Order Addition (no match):**
- p50: 60 ns
- p95: 120 ns
- p99: 200 ns
- p99.9: 500 ns

**Order Cancellation:**
- p50: 40 ns
- p95: 80 ns
- p99: 150 ns
- p99.9: 300 ns

---

## 10. Code Quality

### 10.1 Best Practices Followed

✅ **RAII**: Automatic resource management  
✅ **Const-correctness**: Read-only methods marked const  
✅ **Clear naming**: Self-documenting identifiers  
✅ **Minimal coupling**: Clean interfaces  
✅ **Single responsibility**: Each method has one purpose  
✅ **Documentation**: Comprehensive comments  

### 10.2 Compiler Warnings

All code compiles cleanly with:
```
-Wall -Wextra -pedantic
```

No warnings, no errors.

---

## 11. Conclusion

This implementation provides a solid foundation for a production-grade limit order book. The design prioritizes:

1. **Correctness**: Strict Price-Time Priority
2. **Performance**: Appropriate data structure choices
3. **Clarity**: Readable, maintainable code
4. **Extensibility**: Clear paths for enhancement

The architecture supports scaling to multi-threaded environments with appropriate synchronization primitives and can be extended with advanced order types and market data features as needed.

---

## Appendix A: References

- **Modern C++ Design**: Andrei Alexandrescu
- **Algorithmic Trading**: Ernest Chan
- **CME Market Data**: Price-Time Priority specification
- **FIX Protocol**: Financial Information eXchange
- **Aeron Messaging**: High-performance messaging patterns

## Appendix B: Contact

For questions or improvements, please refer to the project repository.
