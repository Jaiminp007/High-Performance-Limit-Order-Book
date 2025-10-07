# Makefile for High-Performance Limit Order Book

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2

# Debug flags
DEBUGFLAGS = -std=c++17 -Wall -Wextra -pedantic -g -O0 -DDEBUG

# Target executable
TARGET = orderbook

# Debug executable
DEBUG_TARGET = orderbook_debug

# Trading terminal executable
TERMINAL = trading_terminal

# Source files
SOURCES = main.cpp OrderBook.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Header files
HEADERS = OrderBook.h

# Default target
all: $(TARGET) $(TERMINAL)

# Build release version
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	@echo "Build successful! Run with: ./$(TARGET)"

# Build trading terminal
$(TERMINAL): TradingTerminal.cpp OrderBook.cpp OrderBook.h
	$(CXX) $(CXXFLAGS) -o $(TERMINAL) TradingTerminal.cpp OrderBook.cpp
	@echo "Trading terminal built! Run with: ./$(TERMINAL)"

# Build just the terminal
terminal: $(TERMINAL)

# Build debug version
debug: CXXFLAGS = $(DEBUGFLAGS)
debug: clean
	$(CXX) $(DEBUGFLAGS) -o $(DEBUG_TARGET) $(SOURCES)
	@echo "Debug build successful! Run with: ./$(DEBUG_TARGET)"

# Pattern rule for object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run trading terminal
run-terminal: $(TERMINAL)
	./$(TERMINAL)

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) $(DEBUG_TARGET) $(TERMINAL)
	@echo "Clean complete"

# Rebuild from scratch
rebuild: clean all

# Performance build with aggressive optimizations
performance: CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -march=native -flto
performance: clean $(TARGET)
	@echo "Performance build complete!"

# Static analysis
analyze:
	$(CXX) $(CXXFLAGS) -fsyntax-only $(SOURCES)
	@echo "Static analysis complete"

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build release version (default)"
	@echo "  debug        - Build debug version with symbols"
	@echo "  performance  - Build with aggressive optimizations"
	@echo "  run          - Build and run the program"
	@echo "  terminal     - Build interactive trading terminal"
	@echo "  run-terminal - Build and run trading terminal"
	@echo "  clean        - Remove build artifacts"
	@echo "  rebuild      - Clean and build from scratch"
	@echo "  analyze      - Run static analysis"
	@echo "  help         - Show this help message"

.PHONY: all debug run terminal run-terminal clean rebuild performance analyze help
