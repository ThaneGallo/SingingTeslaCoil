# Makefile

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Werror

# Target executable
TARGET = main

# Source files
SRCS = main.c utils.c midi_event.c

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) -lm

# Clean target to remove the executable
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
