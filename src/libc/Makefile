# === Configuration ===

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
PREFIX = /usr/z80-elf

# Compiler and tools
CC = ez80-clang
AS = z80-elf-as
AR = z80-elf-ar

# Flags
CFLAGS = -target z80-elf -nostdinc --std=c90
ASFLAGS =
ARFLAGS = rcs

# Files
C_SOURCES = $(shell find $(SRC_DIR) -name '*.c')
C_INCLUDES = $(shell find $(SRC_DIR) -name '*.h')
ASM_SOURCES = $(shell find $(SRC_DIR) -name '*.s')

C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.s,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Target
TARGET = $(BUILD_DIR)/libc.a

# === Rules ===

.PHONY: all clean install

all: $(TARGET)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(BUILD_DIR)

install: $(TARGET)
	mkdir -p $(PREFIX)/lib
	cp $(TARGET) $(PREFIX)/lib
	mkdir -p $(PREFIX)/include
	cp $(C_INCLUDES) $(PREFIX)/include

$(TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(C_INCLUDES)
	$(CC) $(CFLAGS) -S -o $(OBJ_DIR)/$*.s $<
	sed -i 's/_\(.*\)/\1/g' $(OBJ_DIR)/$*.s
	sed -i 's/public/\.global/g' $(OBJ_DIR)/$*.s
	sed -i 's/or\ta, a/or a/g' $(OBJ_DIR)/$*.s
	$(AS) $(ASFLAGS) -o $@ $(OBJ_DIR)/$*.s
	rm $(OBJ_DIR)/$*.s

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s | $(OBJ_DIR)
	$(AS) $(ASFLAGS) -o $@ $<
