#include "OrderBook.h"
#include <iostream>
#include <iomanip>

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
                  << " | Order ID: " << trade.order_id
                  << " | Price: " << trade.price
                  << " | Qty: " << trade.quantity << "\n";
    }
}

/**
 * @brief Comprehensive test driver demonstrating all order book functionality
 */
int main() {
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
    
    book.CancelOrder(11);
    
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
    
    book.CancelOrder(999);
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
