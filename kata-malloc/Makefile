CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11
LDFLAGS = -lm
TARGET = benchmark
SOURCE = benchmark.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE) $(LDFLAGS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean 