#include "OrderBook.h"

OrderBook::OrderBook() : next_trade_id_(1) {}

std::vector<Trade> OrderBook::AddOrder(uint64_t order_id, Side side, uint64_t price, uint64_t quantity) {
    // Create the order
    Order order(order_id, side, price, quantity);
    
    // Attempt to match the order
    std::vector<Trade> trades;
    if (side == Side::BUY) {
        trades = MatchBuyOrder(order);
    } else {
        trades = MatchSellOrder(order);
    }
    
    // If there's remaining quantity, add it as a resting order
    if (order.quantity > 0) {
        AddRestingOrder(order);
    }
    
    return trades;
}

void OrderBook::CancelOrder(uint64_t order_id) {
    // Check if the order exists
    auto it = order_lookup_.find(order_id);
    if (it == order_lookup_.end()) {
        // Order not found - could log this in production
        return;
    }
    
    // Get the order location
    const OrderLocation& location = it->second;
    
    // Determine which side of the book to remove from
    Side side = location.iterator->side;
    
    // Remove the order from the book
    RemoveOrder(location.price, location.iterator, side);
}

void OrderBook::PrintOrderBook() const {
    std::cout << "\n========== ORDER BOOK ==========\n";
    
    // Print asks in reverse order (highest to lowest)
    std::cout << "\n--- ASKS (SELL ORDERS) ---\n";
    if (asks_.empty()) {
        std::cout << "  (empty)\n";
    } else {
        // Print from highest price to lowest
        for (auto rit = asks_.rbegin(); rit != asks_.rend(); ++rit) {
            uint64_t price = rit->first;
            const OrderList& orders = rit->second;
            
            // Calculate total quantity at this price level
            uint64_t total_qty = 0;
            for (const auto& order : orders) {
                total_qty += order.quantity;
            }
            
            std::cout << "  Price: " << price << " | Qty: " << total_qty 
                      << " | Orders: " << orders.size() << "\n";
        }
    }
    
    std::cout << "\n--- SPREAD ---\n";
    if (!asks_.empty() && !bids_.empty()) {
        uint64_t best_ask = asks_.begin()->first;
        uint64_t best_bid = bids_.begin()->first;
        std::cout << "  Best Ask: " << best_ask << " | Best Bid: " << best_bid 
                  << " | Spread: " << (best_ask - best_bid) << "\n";
    } else {
        std::cout << "  (no spread - one side empty)\n";
    }
    
    // Print bids (highest to lowest, already sorted)
    std::cout << "\n--- BIDS (BUY ORDERS) ---\n";
    if (bids_.empty()) {
        std::cout << "  (empty)\n";
    } else {
        for (const auto& [price, orders] : bids_) {
            // Calculate total quantity at this price level
            uint64_t total_qty = 0;
            for (const auto& order : orders) {
                total_qty += order.quantity;
            }
            
            std::cout << "  Price: " << price << " | Qty: " << total_qty 
                      << " | Orders: " << orders.size() << "\n";
        }
    }
    
    std::cout << "\n================================\n";
}

std::vector<Trade> OrderBook::MatchBuyOrder(Order& order) {
    std::vector<Trade> trades;
    
    // Match against asks (sell orders), starting with the lowest price
    while (order.quantity > 0 && !asks_.empty()) {
        auto& [ask_price, ask_orders] = *asks_.begin();
        
        // Check if we can match at this price level
        if (order.price < ask_price) {
            // Buy order price is too low, no more matches possible
            break;
        }
        
        // Match against orders at this price level (FIFO)
        while (order.quantity > 0 && !ask_orders.empty()) {
            Order& resting_order = ask_orders.front();
            
            // Determine execution quantity
            uint64_t exec_qty = std::min(order.quantity, resting_order.quantity);
            
            // Create a trade
            trades.emplace_back(next_trade_id_++, resting_order.id, ask_price, exec_qty);
            
            // Update quantities
            order.quantity -= exec_qty;
            resting_order.quantity -= exec_qty;
            
            // If the resting order is fully filled, remove it
            if (resting_order.quantity == 0) {
                order_lookup_.erase(resting_order.id);
                ask_orders.pop_front();
            }
        }
        
        // If all orders at this price level are consumed, remove the price level
        if (ask_orders.empty()) {
            asks_.erase(asks_.begin());
        }
    }
    
    return trades;
}

std::vector<Trade> OrderBook::MatchSellOrder(Order& order) {
    std::vector<Trade> trades;
    
    // Match against bids (buy orders), starting with the highest price
    while (order.quantity > 0 && !bids_.empty()) {
        auto& [bid_price, bid_orders] = *bids_.begin();
        
        // Check if we can match at this price level
        if (order.price > bid_price) {
            // Sell order price is too high, no more matches possible
            break;
        }
        
        // Match against orders at this price level (FIFO)
        while (order.quantity > 0 && !bid_orders.empty()) {
            Order& resting_order = bid_orders.front();
            
            // Determine execution quantity
            uint64_t exec_qty = std::min(order.quantity, resting_order.quantity);
            
            // Create a trade
            trades.emplace_back(next_trade_id_++, resting_order.id, bid_price, exec_qty);
            
            // Update quantities
            order.quantity -= exec_qty;
            resting_order.quantity -= exec_qty;
            
            // If the resting order is fully filled, remove it
            if (resting_order.quantity == 0) {
                order_lookup_.erase(resting_order.id);
                bid_orders.pop_front();
            }
        }
        
        // If all orders at this price level are consumed, remove the price level
        if (bid_orders.empty()) {
            bids_.erase(bids_.begin());
        }
    }
    
    return trades;
}

void OrderBook::AddRestingOrder(const Order& order) {
    if (order.side == Side::BUY) {
        // Add to bids
        auto& order_list = bids_[order.price];
        order_list.push_back(order);
        
        // Store location for fast cancellation
        auto it = std::prev(order_list.end());
        order_lookup_[order.id] = {order.price, it};
    } else {
        // Add to asks
        auto& order_list = asks_[order.price];
        order_list.push_back(order);
        
        // Store location for fast cancellation
        auto it = std::prev(order_list.end());
        order_lookup_[order.id] = {order.price, it};
    }
}

void OrderBook::RemoveOrder(uint64_t price, OrderIterator it, Side side) {
    if (side == Side::BUY) {
        auto bid_it = bids_.find(price);
        if (bid_it != bids_.end()) {
            bid_it->second.erase(it);
            
            // Remove price level if empty
            if (bid_it->second.empty()) {
                bids_.erase(bid_it);
            }
        }
    } else {
        auto ask_it = asks_.find(price);
        if (ask_it != asks_.end()) {
            ask_it->second.erase(it);
            
            // Remove price level if empty
            if (ask_it->second.empty()) {
                asks_.erase(ask_it);
            }
        }
    }
    
    // Remove from lookup
    order_lookup_.erase(it->id);
}
