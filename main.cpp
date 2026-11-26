#include "OrderBook.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

/**
 * @brief Helper function to print trades
 */
void PrintTrades(const std::vector<Trade>& trades) {
    if (trades.empty()) {
        std::cout << "  No trades executed.\n";
        return;
    }
    
    std::cout << "  Trades executed:\n";
    for (const auto& trade : trades) {
        std::cout << "    Trade #" << trade.trade_id 
                  << " | Buy ID: " << trade.buy_order_id
                  << " | Sell ID: " << trade.sell_order_id
                  << " | Price: " << trade.price
                  << " | Qty: " << trade.quantity << "\n";
    }
}

void Expect(bool condition, const char* message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

template <typename Fn>
void ExpectThrows(Fn&& fn, const char* message) {
    try {
        fn();
    } catch (const std::invalid_argument&) {
        return;
    }
    throw std::runtime_error(message);
}

void RunCorrectnessTests() {
    {
        OrderBook book;
        Expect(book.AddOrder(1, Side::SELL, 100, 10).empty(), "resting sell should not match empty book");
        auto trades = book.AddOrder(2, Side::BUY, 100, 4);
        Expect(trades.size() == 1, "expected one buy/sell trade");
        Expect(trades[0].buy_order_id == 2, "trade must report buy order ID");
        Expect(trades[0].sell_order_id == 1, "trade must report sell order ID");
        Expect(trades[0].price == 100 && trades[0].quantity == 4, "trade price/quantity mismatch");
        Expect(book.CancelOrder(1), "partially filled resting order should cancel");
        Expect(!book.CancelOrder(1), "duplicate cancellation should return false");
        Expect(!book.CancelOrder(999), "missing cancellation should return false");
    }

    {
        OrderBook book;
        book.AddOrder(1, Side::BUY, 100, 10);
        ExpectThrows([&] { book.AddOrder(1, Side::BUY, 101, 1); }, "duplicate active order ID accepted");
        ExpectThrows([&] { book.AddOrder(3, Side::BUY, 100, 0); }, "zero-quantity order accepted");
        ExpectThrows([&] { book.AddOrder(4, Side::SELL, 0, 1); }, "zero-price limit order accepted");
        ExpectThrows([&] { book.AddMarketOrder(5, Side::BUY, 0); }, "zero-quantity market order accepted");
    }

    {
        OrderBook book;
        Expect(book.AddMarketOrder(1, Side::BUY, 10).empty(), "empty-book market buy should not execute");
        Expect(book.AddOrder(2, Side::SELL, 100, 1).empty(), "unfilled market buy incorrectly rested in book");
    }

    {
        OrderBook book;
        book.AddOrder(1, Side::SELL, 100, 5);
        auto trades = book.AddMarketOrder(2, Side::BUY, 10);
        Expect(trades.size() == 1 && trades[0].quantity == 5, "partial market buy fill mismatch");
        Expect(book.AddOrder(3, Side::SELL, 100, 1).empty(), "unfilled market buy remainder incorrectly rested");
    }

    {
        OrderBook book;
        book.AddOrder(1, Side::SELL, 100, 10);
        book.AddOrder(2, Side::SELL, 100, 10);
        auto trades = book.AddOrder(3, Side::BUY, 100, 15);
        Expect(trades.size() == 2, "FIFO test should produce two trades");
        Expect(trades[0].sell_order_id == 1 && trades[0].quantity == 10, "FIFO priority not respected at first fill");
        Expect(trades[1].sell_order_id == 2 && trades[1].quantity == 5, "FIFO priority not respected at second fill");
    }
}

/**
 * @brief Comprehensive test driver demonstrating all order book functionality
 */
int main() {
    try {
        RunCorrectnessTests();
        std::cout << "Correctness checks passed.\n";
    } catch (const std::exception& error) {
        std::cerr << "Correctness check failed: " << error.what() << "\n";
        return 1;
    }

    std::cout << "=================================================\n";
    std::cout << "  HIGH-FREQUENCY TRADING ORDER BOOK SIMULATOR\n";
    std::cout << "=================================================\n";
    
    // Create an order book
    OrderBook book;
    
    // ========== STEP 1: Build initial order book ==========
    std::cout << "\n[STEP 1] Building initial order book with resting orders...\n";
    
    // Add resting BUY orders (bids)
    std::cout << "\nAdding BUY orders:\n";
    book.AddOrder(1, Side::BUY, 100, 50);   // Bid at 100 for 50 shares
    std::cout << "  Order #1: BUY 50 @ 100\n";
    
    book.AddOrder(2, Side::BUY, 100, 30);   // Another bid at 100 for 30 shares
    std::cout << "  Order #2: BUY 30 @ 100\n";
    
    book.AddOrder(3, Side::BUY, 99, 40);    // Bid at 99 for 40 shares
    std::cout << "  Order #3: BUY 40 @ 99\n";
    
    book.AddOrder(4, Side::BUY, 98, 60);    // Bid at 98 for 60 shares
    std::cout << "  Order #4: BUY 60 @ 98\n";
    
    // Add resting SELL orders (asks)
    std::cout << "\nAdding SELL orders:\n";
    book.AddOrder(5, Side::SELL, 101, 25);  // Ask at 101 for 25 shares
    std::cout << "  Order #5: SELL 25 @ 101\n";
    
    book.AddOrder(6, Side::SELL, 101, 35);  // Another ask at 101 for 35 shares
    std::cout << "  Order #6: SELL 35 @ 101\n";
    
    book.AddOrder(7, Side::SELL, 102, 45);  // Ask at 102 for 45 shares
    std::cout << "  Order #7: SELL 45 @ 102\n";
    
    book.AddOrder(8, Side::SELL, 103, 70);  // Ask at 103 for 70 shares
    std::cout << "  Order #8: SELL 70 @ 103\n";
    
    // Print initial state
    std::cout << "\n[Initial Order Book State]";
    book.PrintOrderBook();
    
    // ========== STEP 2: Aggressive SELL order ==========
    std::cout << "\n\n[STEP 2] Adding aggressive SELL order that crosses the spread...\n";
    std::cout << "  Order #9: SELL 100 @ 99\n";
    std::cout << "  (This should match with bids at 100 and 99)\n\n";
    
    auto trades = book.AddOrder(9, Side::SELL, 99, 100);
    
    std::cout << "Results:\n";
    PrintTrades(trades);
    
    std::cout << "\n[Order Book After Aggressive SELL]";
    book.PrintOrderBook();
    
    // ========== STEP 3: Aggressive BUY order ==========
    std::cout << "\n\n[STEP 3] Adding aggressive BUY order that is partially filled...\n";
    std::cout << "  Order #10: BUY 100 @ 102\n";
    std::cout << "  (This should match all asks at 101 and partially match at 102)\n\n";
    
    trades = book.AddOrder(10, Side::BUY, 102, 100);
    
    std::cout << "Results:\n";
    PrintTrades(trades);
    
    std::cout << "\n[Order Book After Aggressive BUY]";
    book.PrintOrderBook();
    
    // ========== STEP 4: Add more resting orders ==========
    std::cout << "\n\n[STEP 4] Adding new resting orders...\n";
    
    std::cout << "\nAdding new orders:\n";
    book.AddOrder(11, Side::BUY, 101, 20);
    std::cout << "  Order #11: BUY 20 @ 101\n";
    
    book.AddOrder(12, Side::SELL, 104, 80);
    std::cout << "  Order #12: SELL 80 @ 104\n";
    
    book.AddOrder(13, Side::BUY, 100, 15);
    std::cout << "  Order #13: BUY 15 @ 100\n";
    
    std::cout << "\n[Order Book After Adding New Orders]";
    book.PrintOrderBook();
    
    // ========== STEP 5: Cancel an order ==========
    std::cout << "\n\n[STEP 5] Cancelling an existing order...\n";
    std::cout << "  Cancelling Order #11 (BUY 20 @ 101)\n";
    
    Expect(book.CancelOrder(11), "expected order 11 to cancel in demonstration");
    
    std::cout << "\n[Order Book After Cancellation]";
    book.PrintOrderBook();
    
    // ========== STEP 6: Market sweep with large order ==========
    std::cout << "\n\n[STEP 6] Market sweep with large BUY order...\n";
    std::cout << "  Order #14: BUY 500 @ 110\n";
    std::cout << "  (This should sweep through all ask levels and become resting)\n\n";
    
    trades = book.AddOrder(14, Side::BUY, 110, 500);
    
    std::cout << "Results:\n";
    PrintTrades(trades);
    
    std::cout << "\n[Order Book After Market Sweep]";
    book.PrintOrderBook();
    
    // ========== STEP 7: Test invalid cancellation ==========
    std::cout << "\n\n[STEP 7] Testing cancellation of non-existent order...\n";
    std::cout << "  Attempting to cancel Order #999 (doesn't exist)\n";
    
    Expect(!book.CancelOrder(999), "missing order cancellation should return false");
    std::cout << "  Cancellation handled gracefully (order not found)\n";
    
    // ========== STEP 8: Add orders that match immediately ==========
    std::cout << "\n\n[STEP 8] Adding orders that match immediately and completely...\n";
    std::cout << "  Order #15: SELL 50 @ 98 (should match immediately)\n\n";
    
    trades = book.AddOrder(15, Side::SELL, 98, 50);
    
    std::cout << "Results:\n";
    PrintTrades(trades);
    
    std::cout << "\n[Final Order Book State]";
    book.PrintOrderBook();
    
    // ========== STEP 9: Performance characteristics ==========
    std::cout << "\n\n[STEP 9] Performance characteristics:\n";
    std::cout << "  - Order insertion: O(log n) - using std::map for price levels\n";
    std::cout << "  - Order cancellation: O(1) average - using std::unordered_map lookup\n";
    std::cout << "  - Matching: O(m) where m is number of orders matched\n";
    std::cout << "  - Price-Time Priority: Maintained via FIFO std::list at each price level\n";
    std::cout << "  - Memory efficiency: Only active orders stored, no pre-allocation\n";
    
    std::cout << "\n\n=================================================\n";
    std::cout << "  TEST COMPLETED SUCCESSFULLY\n";
    std::cout << "=================================================\n";
    
    return 0;
}
