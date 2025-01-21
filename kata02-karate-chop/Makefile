# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++2a

# Source files
SRCS = main.cpp monday.cpp

# Object files directory
BUILD_DIR = build

# Executable directory
BIN_DIR = bin

# Object files
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Executable name
TARGET = $(BIN_DIR)/karate_chop

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Run the program
run: $(TARGET)
	@echo "Running: ./$(TARGET) monday"
	./$(TARGET) monday

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
