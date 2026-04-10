# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

# Target executable name
TARGET = dispatcher

# Source files
SRCS = Dispatcher.c
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = Process.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "Cleaned build artifacts"

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run with valgrind memory checking (if available)
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET)

# Debug build with extra symbols
debug: CFLAGS += -DDEBUG -O0
debug: clean $(TARGET)

# Create a release build (optimized)
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Check for memory leaks with AddressSanitizer
sanitize: CFLAGS += -fsanitize=address -g
sanitize: LDFLAGS += -fsanitize=address
sanitize: clean $(TARGET)
	./$(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  make          - Build the dispatcher program"
	@echo "  make run      - Build and run the program"
	@echo "  make clean    - Remove compiled files"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make release  - Build with optimizations"
	@echo "  make sanitize - Build with AddressSanitizer for memory checking"
	@echo "  make valgrind - Run with valgrind (if installed)"
	@echo "  make help     - Show this help message"

.PHONY: all clean run valgrind debug release sanitize help
