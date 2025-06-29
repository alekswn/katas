CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -g -Wno-alloc-size-larger-than
LDFLAGS = 

# Test binary
TEST_BIN = test-malloc

# Source files
TEST_SRC = test-malloc.c

# Default target
all: $(TEST_BIN)

# Build test binary
$(TEST_BIN): $(TEST_SRC)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Test with system malloc
test-system: $(TEST_BIN)
	./$(TEST_BIN)

# Clean build artifacts
clean:
	rm -f $(TEST_BIN)

# Install dependencies (if needed)
install-deps:
	# No external dependencies required

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build test binary"
	@echo "  test-system  - Run tests with system malloc"
	@echo "  test-naive   - Run tests with naive malloc (requires naive-malloc library)"
	@echo "  clean        - Remove build artifacts"
	@echo "  help         - Show this help message"

.PHONY: all test-system test-naive clean install-deps help 