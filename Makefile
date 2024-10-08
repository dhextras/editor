# Compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -pedantic

# Source & Object files
SRC_DIR = src
OBJ_DIR = build
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/gapbuffer.c $(SRC_DIR)/memory.c $(SRC_DIR)/screen.c $(SRC_DIR)/terminal.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Target executable
TARGET = $(OBJ_DIR)/editor
$(shell mkdir -p $(OBJ_DIR))

all: $(TARGET)

# Linking the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Compiling source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove compiled files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
