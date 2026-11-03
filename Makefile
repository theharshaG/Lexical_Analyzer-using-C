CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = lexer
SRCS = main.c lexer.c syntax.c
OBJS = $(SRCS:.c=.o)
HDRS = lexer.h syntax.h

# Default target: compile the lexer executable
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compile each .c file into an object file
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Convenience target to compile and run with default input.c
run: $(TARGET)
	./$(TARGET)

# Remove build artifacts and binaries
clean:
	rm -f $(OBJS) $(TARGET) a.out

.PHONY: all run clean
