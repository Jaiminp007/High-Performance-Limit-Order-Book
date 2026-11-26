#include "OrderBook.h"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace std;

class TradingTerminal {
private:
    static constexpr uint64_t FIRST_ORDER_ID = 1000;

    OrderBook book;
    uint64_t orderIdCounter;
    uint64_t totalOrders;
    uint64_t totalVolume;
    uint64_t totalTrades;

    string getCurrentTime() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        auto us = chrono::duration_cast<chrono::microseconds>(
            now.time_since_epoch()) % 1000000;

        tm local{};
#ifdef _WIN32
        localtime_s(&local, &time);
#else
        localtime_r(&time, &local);
#endif

        stringstream ss;
        ss << put_time(&local, "%H:%M:%S");
        ss << "." << setfill('0') << setw(6) << us.count();
        return ss.str();
    }

    vector<Trade> submitLimitOrder(Side side, uint64_t price, uint64_t quantity) {
        auto trades = book.AddOrder(orderIdCounter++, side, price, quantity);
        totalOrders++;
        return trades;
    }

    vector<Trade> submitMarketOrder(Side side, uint64_t quantity) {
        auto trades = book.AddMarketOrder(orderIdCounter++, side, quantity);
        totalOrders++;
        return trades;
    }

    static uint64_t executedQuantity(const vector<Trade>& trades) {
        uint64_t total = 0;
        for (const auto& trade : trades) {
            total += trade.quantity;
        }
        return total;
    }

    bool readPositiveUint64(const string& prompt, uint64_t& value) {
        while (true) {
            cout << prompt;
            string input;
            if (!(cin >> input)) {
                cout << "\nInput closed.\n";
                return false;
            }

            const bool digits_only = !input.empty() && all_of(input.begin(), input.end(), [](unsigned char c) {
                return isdigit(c) != 0;
            });
            if (!digits_only) {
                cout << "Invalid number. Enter a positive integer.\n";
                continue;
            }

            try {
                size_t parsed_chars = 0;
                const unsigned long long parsed = stoull(input, &parsed_chars);
                if (parsed_chars != input.size() || parsed == 0 ||
                    parsed > numeric_limits<uint64_t>::max()) {
                    throw out_of_range("invalid order value");
                }
                value = static_cast<uint64_t>(parsed);
                return true;
            } catch (const exception&) {
                cout << "Invalid number. Enter a positive integer.\n";
            }
        }
    }

    void printHeader() {
        cout << "\n";
        cout << "================================================================================\n";
        cout << "                        QUANTUM TRADING TERMINAL v2.1                           \n";
        cout << "                        Real-Time Order Book Engine                             \n";
        cout << "================================================================================\n";
        cout << "\n";
    }

    void printStats() {
        cout << "\n--- SESSION STATISTICS ---\n";
        cout << "Total Orders: " << totalOrders << " | ";
        cout << "Trades: " << totalTrades << " | ";
        cout << "Volume: " << totalVolume << "\n";
        cout << "Timestamp: " << getCurrentTime() << "\n\n";
    }

    void displayTrades(const vector<Trade>& trades) {
        if (trades.empty()) {
            cout << "[" << getCurrentTime() << "] No executions\n";
            return;
        }

        cout << "\n+++ EXECUTION REPORT +++\n";
        cout << left << setw(17) << "Time"
             << setw(10) << "Trade ID"
             << setw(10) << "Buy ID"
             << setw(10) << "Sell ID"
             << setw(10) << "Price"
             << setw(10) << "Qty" << "\n";
        cout << string(67, '-') << "\n";

        for (const auto& t : trades) {
            cout << left << setw(17) << getCurrentTime()
                 << setw(10) << t.trade_id
                 << setw(10) << t.buy_order_id
                 << setw(10) << t.sell_order_id
                 << setw(10) << t.price
                 << setw(10) << t.quantity << "\n";
            totalTrades++;
            totalVolume += t.quantity;
        }
        cout << "\n";
    }

    void showMenu() {
        cout << "\n=== ACTIONS ===\n";
        cout << "1) Market Buy     2) Market Sell\n";
        cout << "3) Limit Buy      4) Limit Sell\n";
        cout << "5) Cancel Order   6) View Book\n";
        cout << "7) Run Demo       8) Clear Screen\n";
        cout << "9) Statistics     0) Exit\n";
        cout << "\nChoice: ";
    }

    void runDemo() {
        cout << "\n[DEMO MODE] Initializing market simulation...\n\n";
        this_thread::sleep_for(chrono::milliseconds(500));

        cout << "Adding initial liquidity...\n";
        submitLimitOrder(Side::BUY, 9950, 100);
        submitLimitOrder(Side::BUY, 9940, 150);
        submitLimitOrder(Side::BUY, 9930, 200);
        submitLimitOrder(Side::SELL, 10050, 100);
        submitLimitOrder(Side::SELL, 10060, 150);
        submitLimitOrder(Side::SELL, 10070, 200);

        this_thread::sleep_for(chrono::milliseconds(300));
        book.PrintOrderBook();

        cout << "\n[DEMO] Executing aggressive buy order...\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        auto trades = submitLimitOrder(Side::BUY, 10060, 120);
        displayTrades(trades);

        this_thread::sleep_for(chrono::milliseconds(300));
        book.PrintOrderBook();

        cout << "\n[DEMO] Demo completed successfully\n";
    }

    void clearScreen() {
        cout << "\033[2J\033[H";
        printHeader();
    }

public:
    TradingTerminal()
        : orderIdCounter(FIRST_ORDER_ID), totalOrders(0), totalVolume(0), totalTrades(0) {}

    void run() {
        clearScreen();

        cout << "System initialized at " << getCurrentTime() << "\n";
        cout << "Connected to matching engine...\n";
        this_thread::sleep_for(chrono::milliseconds(300));
        cout << "Ready for trading.\n";

        bool running = true;
        while (running) {
            showMenu();

            string choiceInput;
            if (!(cin >> choiceInput)) {
                cout << "\nInput closed.\n";
                break;
            }

            int choice = -1;
            try {
                size_t parsed_chars = 0;
                choice = stoi(choiceInput, &parsed_chars);
                if (parsed_chars != choiceInput.size()) {
                    throw invalid_argument("trailing input");
                }
            } catch (const exception&) {
                cout << "\nInvalid input. Try again.\n";
                continue;
            }

            if (choice == 0) {
                cout << "\nClosing session...\n";
                printStats();
                cout << "Goodbye.\n\n";
                break;
            }

            uint64_t price = 0;
            uint64_t qty = 0;
            vector<Trade> trades;

            switch (choice) {
                case 1: // Market Buy
                    if (!readPositiveUint64("Quantity: ", qty)) {
                        running = false;
                        break;
                    }
                    trades = submitMarketOrder(Side::BUY, qty);
                    displayTrades(trades);
                    if (executedQuantity(trades) < qty) {
                        cout << "Unfilled quantity cancelled (market orders do not rest).\n";
                    }
                    break;

                case 2: // Market Sell
                    if (!readPositiveUint64("Quantity: ", qty)) {
                        running = false;
                        break;
                    }
                    trades = submitMarketOrder(Side::SELL, qty);
                    displayTrades(trades);
                    if (executedQuantity(trades) < qty) {
                        cout << "Unfilled quantity cancelled (market orders do not rest).\n";
                    }
                    break;

                case 3: // Limit Buy
                    if (!readPositiveUint64("Price: ", price) ||
                        !readPositiveUint64("Quantity: ", qty)) {
                        running = false;
                        break;
                    }
                    trades = submitLimitOrder(Side::BUY, price, qty);
                    if (trades.empty()) {
                        cout << "[" << getCurrentTime() << "] Limit order posted (ID: "
                             << (orderIdCounter - 1) << ")\n";
                    } else {
                        displayTrades(trades);
                        const uint64_t remaining = qty - executedQuantity(trades);
                        if (remaining > 0) {
                            cout << "Remaining " << remaining << " resting as limit order ID "
                                 << (orderIdCounter - 1) << ".\n";
                        }
                    }
                    break;

                case 4: // Limit Sell
                    if (!readPositiveUint64("Price: ", price) ||
                        !readPositiveUint64("Quantity: ", qty)) {
                        running = false;
                        break;
                    }
                    trades = submitLimitOrder(Side::SELL, price, qty);
                    if (trades.empty()) {
                        cout << "[" << getCurrentTime() << "] Limit order posted (ID: "
                             << (orderIdCounter - 1) << ")\n";
                    } else {
                        displayTrades(trades);
                        const uint64_t remaining = qty - executedQuantity(trades);
                        if (remaining > 0) {
                            cout << "Remaining " << remaining << " resting as limit order ID "
                                 << (orderIdCounter - 1) << ".\n";
                        }
                    }
                    break;

                case 5: { // Cancel
                    uint64_t cancelId = 0;
                    if (!readPositiveUint64("Order ID to cancel: ", cancelId)) {
                        running = false;
                        break;
                    }
                    if (book.CancelOrder(cancelId)) {
                        cout << "[" << getCurrentTime() << "] Cancelled order "
                             << cancelId << "\n";
                    } else {
                        cout << "[" << getCurrentTime() << "] Order " << cancelId
                             << " not found or already filled\n";
                    }
                    break;
                }

                case 6: // View Book
                    book.PrintOrderBook();
                    break;

                case 7: // Demo
                    runDemo();
                    break;

                case 8: // Clear
                    clearScreen();
                    break;

                case 9: // Stats
                    printStats();
                    break;

                default:
                    cout << "\nUnknown option.\n";
            }
        }
    }
};

int main() {
    TradingTerminal terminal;
    terminal.run();
    return 0;
}
