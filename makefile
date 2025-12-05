# Define project name
PROJECT_NAME := kvdb

# Define directories
SRC_DIR := src
INCLUDE_DIR := include
BIN_DIR := bin
OBJ_DIR := obj
DATA_DIR := data

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -I$(INCLUDE_DIR)

# Find all C source files in SRC_DIR
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Generate object file names in OBJ_DIR
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

EXECUTABLE := $(BIN_DIR)/$(PROJECT_NAME)

# Main target: build the executable
.PHONY: all
all: $(EXECUTABLE)

# Rule to link object files into the executable
$(EXECUTABLE): $(OBJS)
	@mkdir -p $(DATA_DIR)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@

# Rule to compile C source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Phony target for cleaning up generated files
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(DATA_DIR)

.PHONY: dropdb
dropdb:
	rm -rf $(DATA_DIR); mkdir -p $(DATA_DIR)

.PHONY: test
test: $(EXECUTABLE)
	@rm -rf $(DATA_DIR); mkdir -p $(DATA_DIR)
	./$(EXECUTABLE) test hash
	@rm -rf $(DATA_DIR); mkdir -p $(DATA_DIR)
	./$(EXECUTABLE) test setget
	@rm -rf $(DATA_DIR); mkdir -p $(DATA_DIR)
	./$(EXECUTABLE) test del
	@rm -rf $(DATA_DIR); mkdir -p $(DATA_DIR)
	./$(EXECUTABLE) test ts
	@rm -rf $(DATA_DIR); mkdir -p $(DATA_DIR)
