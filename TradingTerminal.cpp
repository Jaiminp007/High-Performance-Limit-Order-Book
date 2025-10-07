#include "OrderBook.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <thread>
#include <ctime>

using namespace std;

class TradingTerminal {
private:
    OrderBook book;
    uint64_t orderIdCounter;
    uint64_t totalVolume;
    int totalTrades;
    
    string getCurrentTime() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        auto ms = chrono::duration_cast<chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        stringstream ss;
        ss << put_time(localtime(&time), "%H:%M:%S");
        ss << "." << setfill('0') << setw(3) << ms.count();
        return ss.str();
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
        cout << "Total Orders: " << (orderIdCounter - 1) << " | ";
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
        cout << left << setw(12) << "Time" 
             << setw(10) << "Trade ID"
             << setw(12) << "Order ID"
             << setw(10) << "Price"
             << setw(10) << "Qty" << "\n";
        cout << string(54, '-') << "\n";
        
        for (const auto& t : trades) {
            cout << left << setw(12) << getCurrentTime()
                 << setw(10) << t.trade_id
                 << setw(12) << t.order_id
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
        
        // Add some initial liquidity
        cout << "Adding initial liquidity...\n";
        book.AddOrder(orderIdCounter++, Side::BUY, 9950, 100);
        book.AddOrder(orderIdCounter++, Side::BUY, 9940, 150);
        book.AddOrder(orderIdCounter++, Side::BUY, 9930, 200);
        book.AddOrder(orderIdCounter++, Side::SELL, 10050, 100);
        book.AddOrder(orderIdCounter++, Side::SELL, 10060, 150);
        book.AddOrder(orderIdCounter++, Side::SELL, 10070, 200);
        
        this_thread::sleep_for(chrono::milliseconds(300));
        book.PrintOrderBook();
        
        cout << "\n[DEMO] Executing aggressive buy order...\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        auto trades = book.AddOrder(orderIdCounter++, Side::BUY, 10060, 120);
        displayTrades(trades);
        
        this_thread::sleep_for(chrono::milliseconds(300));
        book.PrintOrderBook();
        
        cout << "\n[DEMO] Demo completed successfully\n";
    }
    
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        printHeader();
    }

public:
    TradingTerminal() : orderIdCounter(1000), totalVolume(0), totalTrades(0) {}
    
    void run() {
        clearScreen();
        
        cout << "System initialized at " << getCurrentTime() << "\n";
        cout << "Connected to matching engine...\n";
        this_thread::sleep_for(chrono::milliseconds(300));
        cout << "Ready for trading.\n";
        
        int choice;
        while (true) {
            showMenu();
            
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\nInvalid input. Try again.\n";
                continue;
            }
            
            if (choice == 0) {
                cout << "\nClosing session...\n";
                printStats();
                cout << "Goodbye.\n\n";
                break;
            }
            
            uint64_t price, qty;
            vector<Trade> trades;
            
            switch (choice) {
                case 1: // Market Buy
                    cout << "Quantity: ";
                    cin >> qty;
                    cout << "Max Price: ";
                    cin >> price;
                    trades = book.AddOrder(orderIdCounter++, Side::BUY, price, qty);
                    displayTrades(trades);
                    break;
                    
                case 2: // Market Sell
                    cout << "Quantity: ";
                    cin >> qty;
                    cout << "Min Price: ";
                    cin >> price;
                    trades = book.AddOrder(orderIdCounter++, Side::SELL, price, qty);
                    displayTrades(trades);
                    break;
                    
                case 3: // Limit Buy
                    cout << "Price: ";
                    cin >> price;
                    cout << "Quantity: ";
                    cin >> qty;
                    trades = book.AddOrder(orderIdCounter++, Side::BUY, price, qty);
                    if (trades.empty()) {
                        cout << "[" << getCurrentTime() << "] Limit order posted (ID: " 
                             << (orderIdCounter-1) << ")\n";
                    } else {
                        displayTrades(trades);
                    }
                    break;
                    
                case 4: // Limit Sell
                    cout << "Price: ";
                    cin >> price;
                    cout << "Quantity: ";
                    cin >> qty;
                    trades = book.AddOrder(orderIdCounter++, Side::SELL, price, qty);
                    if (trades.empty()) {
                        cout << "[" << getCurrentTime() << "] Limit order posted (ID: " 
                             << (orderIdCounter-1) << ")\n";
                    } else {
                        displayTrades(trades);
                    }
                    break;
                    
                case 5: // Cancel
                    cout << "Order ID to cancel: ";
                    uint64_t cancelId;
                    cin >> cancelId;
                    book.CancelOrder(cancelId);
                    cout << "[" << getCurrentTime() << "] Cancel request sent for order " 
                         << cancelId << "\n";
                    break;
                    
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
