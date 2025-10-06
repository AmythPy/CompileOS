# CompileOS Makefile
# Builds the CompileOS kernel and bootloader

# Compiler and tools
CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy

# Architecture
ARCH = i386

# Compiler flags
CFLAGS := -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -m32 -march=i386 -fno-pic -Isrc
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -nostdlib

# Directories
SRC_DIR = src
KERNEL_DIR = $(SRC_DIR)/kernel
HAL_DIR = $(SRC_DIR)/hal
BOOT_DIR = $(SRC_DIR)/boot
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files (essential only - no HAL for 32-bit compatibility)
KERNEL_SOURCES = \
	$(KERNEL_DIR)/main.c \
	$(KERNEL_DIR)/util.c \
	$(KERNEL_DIR)/browser.c \
	$(KERNEL_DIR)/desktop/desktop.c \
	$(KERNEL_DIR)/window/window_manager.c \
	$(KERNEL_DIR)/network.c \
	$(KERNEL_DIR)/boot/splash.c

DRIVER_SOURCES =

KERNEL_OBJECTS = $(KERNEL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DRIVER_OBJECTS = $(DRIVER_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Target files
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
BOOTLOADER_BIN = $(BUILD_DIR)/bootloader.bin
EMBEDDED_BOOTLOADER_BIN = $(BUILD_DIR)/embedded_bootloader.bin
EMBEDDED_OS_BMP = $(BUILD_DIR)/compileos_embedded.bmp
ISO_IMAGE = $(BUILD_DIR)/compileos.iso
EMBED_TOOL = $(BUILD_DIR)/embed_os

# Default target
all: $(KERNEL_BIN)

# Create build directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/kernel/memory
	mkdir -p $(OBJ_DIR)/kernel/process
	mkdir -p $(OBJ_DIR)/kernel/debugger
	mkdir -p $(OBJ_DIR)/kernel/terminal
	mkdir -p $(OBJ_DIR)/kernel/repl
	mkdir -p $(OBJ_DIR)/kernel/desktop
	mkdir -p $(OBJ_DIR)/kernel/window
	mkdir -p $(OBJ_DIR)/kernel/fs
	mkdir -p $(OBJ_DIR)/kernel/stubs
	mkdir -p $(OBJ_DIR)/kernel/boot
	mkdir -p $(OBJ_DIR)/hal/arch/x86_64
	mkdir -p $(OBJ_DIR)/hal/arch/arm64
	mkdir -p $(OBJ_DIR)/boot
	mkdir -p $(OBJ_DIR)/tools

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm | $(OBJ_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Assemble top-level boot.S (NASM syntax)
$(OBJ_DIR)/bootS.o: $(SRC_DIR)/boot/boot.S | $(OBJ_DIR)
	$(AS) -f elf32 $< -o $@

# Use top-level boot.S for multiboot header/entry
BOOT_ASM_SOURCES = boot.S
# Object for top-level boot.S
BOOT_ASM_OBJECTS = $(OBJ_DIR)/bootS.o

# Create bootloader binary
$(BOOTLOADER_BIN): $(BOOT_ASM_OBJECTS) | $(OBJ_DIR)
	$(OBJCOPY) -O binary $< $@

# Link kernel (multiboot must be first!)
$(KERNEL_BIN): $(BOOT_ASM_OBJECTS) $(KERNEL_OBJECTS) $(DRIVER_OBJECTS) | $(OBJ_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

# Run in QEMU (using ELF multiboot kernel)
run: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN) -m 512M

# Debug in QEMU (using ELF multiboot kernel)
debug: $(KERNEL_BIN)
	qemu-system-i386 -kernel $(KERNEL_BIN) -m 512M -s -S

# Build embedded OS
embedded: $(EMBEDDED_OS_BMP)
	@echo "Embedded OS created: $(EMBEDDED_OS_BMP)"

# Run embedded OS in QEMU
run-embedded: $(EMBEDDED_OS_BMP)
	qemu-system-x86_64 -hda $(EMBEDDED_OS_BMP) -m 512M

# Extract OS from embedded BMP
extract-os: $(EMBED_TOOL)
	$(EMBED_TOOL) extract $(EMBEDDED_OS_BMP) $(BUILD_DIR)/extracted_kernel.bin

# Verify embedded OS
verify-embedded: $(EMBED_TOOL) $(EMBEDDED_OS_BMP)
	$(EMBED_TOOL) verify $(EMBEDDED_OS_BMP)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential nasm grub-pc-bin grub-common xorriso qemu-system-x86

# Help
help:
	@echo "CompileOS Build System"
	@echo "Available targets:"
	@echo "  all          - Build everything (default)"
	@echo "  clean        - Clean build artifacts"
	@echo "  run          - Run in QEMU"
	@echo "  debug        - Run in QEMU with GDB server"
	@echo "  install-deps - Install build dependencies"
	@echo "  help         - Show this help"

.PHONY: all clean run debug install-deps help
