# Compiler
CXX = clang++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++2a

# Source files
SRCS = main.cpp monday.cpp tuesday.cpp wednesday.cpp thursday.cpp friday.cpp benchmark.cpp

# Object files directory
BUILD_DIR = build

# Executable directory
BIN_DIR = bin

# Object files
OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

# Executable name
TARGET = $(BIN_DIR)/karate_chop
BENCHMARK_TARGET = $(BIN_DIR)/benchmark

# Default target
all: $(TARGET) $(BENCHMARK_TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out $(BUILD_DIR)/benchmark.o, $(OBJS))

$(BENCHMARK_TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(filter-out $(BUILD_DIR)/main.o, $(OBJS))

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
	@echo "Running: ./$(TARGET) tuesday"
	./$(TARGET) tuesday
	@echo "Running: ./$(TARGET) wednesday"
	./$(TARGET) wednesday
	@echo "Running: ./$(TARGET) thursday"
	./$(TARGET) thursday
	@echo "Running: ./$(TARGET) friday"
	./$(TARGET) friday

# Run the benchmark
benchmark: $(BENCHMARK_TARGET)
	@echo "Running benchmark"
	./$(BENCHMARK_TARGET)

# Clean up build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
