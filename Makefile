# Makefile for Flux Programming Language

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = flux
SOURCES = main.cpp lexer.cpp parser.cpp ast.cpp interpreter.cpp
BUILD_DIR = build
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
HEADERS = lexer.h parser.h ast.h interpreter.h

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile source files
$(BUILD_DIR)/%.o: %.cpp $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Install (copy to system path, requires admin privileges)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Test the interpreter with example programs
test: $(TARGET)
	@echo "Testing Flux interpreter..."
	@echo "Running hello world example..."
	./$(TARGET) examples/hello.flux
	@echo "Running fibonacci example..."
	./$(TARGET) examples/fibonacci.flux

.PHONY: all clean install uninstall debug test