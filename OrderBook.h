#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include <cstdint>
#include <vector>
#include <map>
#include <list>
#include <unordered_map>
#include <iostream>

/**
 * @brief Enum representing the side of an order
 */
enum class Side {
    BUY,
    SELL
};

/**
 * @brief Structure representing an order in the order book
 */
struct Order {
    uint64_t id;        // Unique order identifier
    Side side;          // BUY or SELL
    uint64_t price;     // Order price
    uint64_t quantity;  // Order quantity
    
    Order(uint64_t id_, Side side_, uint64_t price_, uint64_t quantity_)
        : id(id_), side(side_), price(price_), quantity(quantity_) {}
};

/**
 * @brief Structure representing a trade (execution)
 */
struct Trade {
    uint64_t trade_id;  // Unique trade identifier
    uint64_t order_id;  // ID of the order being matched
    uint64_t price;     // Execution price
    uint64_t quantity;  // Executed quantity
    
    Trade(uint64_t trade_id_, uint64_t order_id_, uint64_t price_, uint64_t quantity_)
        : trade_id(trade_id_), order_id(order_id_), price(price_), quantity(quantity_) {}
};

/**
 * @brief High-performance limit order book with Price-Time Priority matching
 * 
 * This class implements a single-threaded, in-memory limit order book that:
 * - Maintains bids (buy orders) sorted by price descending (highest first)
 * - Maintains asks (sell orders) sorted by price ascending (lowest first)
 * - Processes orders at the same price level in FIFO order
 * - Supports O(log n) order insertion and O(1) order cancellation
 */
class OrderBook {
public:
    /**
     * @brief Construct a new Order Book object
     */
    OrderBook();
    
    /**
     * @brief Add a new order to the book and attempt to match it
     * 
     * @param order_id Unique order identifier
     * @param side BUY or SELL
     * @param price Order price
     * @param quantity Order quantity
     * @return std::vector<Trade> Vector of trades generated from matching
     */
    std::vector<Trade> AddOrder(uint64_t order_id, Side side, uint64_t price, uint64_t quantity);
    
    /**
     * @brief Cancel an existing order from the book
     * 
     * @param order_id The unique identifier of the order to cancel
     */
    void CancelOrder(uint64_t order_id);
    
    /**
     * @brief Print the current state of the order book to console
     */
    void PrintOrderBook() const;

private:
    // Type alias for a list of orders at a specific price level
    using OrderList = std::list<Order>;
    
    // Type alias for iterator to an order in the list
    using OrderIterator = OrderList::iterator;
    
    // Structure to track order location for O(1) cancellation
    struct OrderLocation {
        uint64_t price;           // Price level of the order
        OrderIterator iterator;   // Iterator to the order in the list
    };
    
    // Bids: map from price to list of orders (sorted descending by key)
    // Using std::greater<uint64_t> to sort prices in descending order
    std::map<uint64_t, OrderList, std::greater<uint64_t>> bids_;
    
    // Asks: map from price to list of orders (sorted ascending by key)
    std::map<uint64_t, OrderList> asks_;
    
    // Fast lookup for order cancellation: order_id -> (price, iterator)
    std::unordered_map<uint64_t, OrderLocation> order_lookup_;
    
    // Trade ID counter for generating unique trade identifiers
    uint64_t next_trade_id_;
    
    /**
     * @brief Match an incoming buy order against existing sell orders
     * 
     * @param order The incoming buy order
     * @return std::vector<Trade> Vector of trades generated
     */
    std::vector<Trade> MatchBuyOrder(Order& order);
    
    /**
     * @brief Match an incoming sell order against existing buy orders
     * 
     * @param order The incoming sell order
     * @return std::vector<Trade> Vector of trades generated
     */
    std::vector<Trade> MatchSellOrder(Order& order);
    
    /**
     * @brief Add a resting order to the appropriate side of the book
     * 
     * @param order The order to add as a resting order
     */
    void AddRestingOrder(const Order& order);
    
    /**
     * @brief Remove an order from the book given its location
     * 
     * @param price The price level of the order
     * @param it Iterator to the order in the list
     * @param side The side of the order (BUY or SELL)
     */
    void RemoveOrder(uint64_t price, OrderIterator it, Side side);
};

#endif // ORDERBOOK_H
