# Compiler and flags
CC      := cc
CFLAGS  := -O2 -Wall -Wextra

# Paths
SRC     := compiler.c
BUILD   := ./build
TARGET  := $(BUILD)/compiler

.PHONY: all clean

all: $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(SRC) | $(BUILD)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -rf $(BUILD)
