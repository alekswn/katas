CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99 -fPIC
LDFLAGS = -shared
TARGET = libnaive-malloc.so
SOURCE = naive-malloc.c

# Default target
all: $(TARGET)

# Build shared library
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SOURCE)

# Clean build files
clean:
	rm -f $(TARGET) *.o

# Install to system (optional)
install: $(TARGET)
	sudo cp $(TARGET) /usr/lib/
	sudo ldconfig

# Uninstall
uninstall:
	sudo rm -f /usr/lib/$(TARGET)
	sudo ldconfig

# Test with LD_PRELOAD
test: $(TARGET)
	LD_PRELOAD=./$(TARGET) echo "Testing naive-malloc with LD_PRELOAD"

.PHONY: all clean install uninstall test 
