# CompileOS Makefile
# Builds the CompileOS kernel and bootloader

# Rust toolchain for kernel components
RUSTC = rustc
CARGO = cargo

# Architecture - Use 32-bit for compatibility
ARCH = i386

# Compiler flags for 32-bit freestanding kernel
CFLAGS := -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -m32 -march=i386 -Isrc -Isrc/devtools
ASFLAGS = -f elf32
LDFLAGS = -T linker.ld -nostdlib

# Directories
SRC_DIR = src
KERNEL_DIR = $(SRC_DIR)/kernel
HAL_DIR = $(SRC_DIR)/hal
BOOT_DIR = $(SRC_DIR)/boot
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files that actually exist
KERNEL_SOURCES = \
	$(KERNEL_DIR)/main.c \
	$(KERNEL_DIR)/util.c \
	$(KERNEL_DIR)/vga_graphics.c \
	$(KERNEL_DIR)/desktop/desktop.c \
	$(KERNEL_DIR)/apps/browser.c \
	$(KERNEL_DIR)/network.c \
	$(KERNEL_DIR)/terminal/terminal.c \
	$(KERNEL_DIR)/fs/fs.c \
	$(KERNEL_DIR)/window/window_manager.c \
	$(KERNEL_DIR)/interrupts.c \
	$(KERNEL_DIR)/timer.c \
	$(KERNEL_DIR)/memory/kmalloc.c \
	$(KERNEL_DIR)/process.c \
	$(KERNEL_DIR)/syscall.c

DRIVER_SOURCES = \
	$(SRC_DIR)/drivers/vga/vga.c \
	$(SRC_DIR)/drivers/keyboard/keyboard.c \
	$(SRC_DIR)/drivers/graphics/graphics.c \
	$(SRC_DIR)/drivers/network/network.c \
	$(SRC_DIR)/drivers/pci/pci.c

# Network stack sources (non-kernel dir)
NET_SOURCES := $(wildcard $(SRC_DIR)/net/*.c)

NET_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(NET_SOURCES))

KERNEL_OBJECTS = $(KERNEL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DRIVER_OBJECTS = $(DRIVER_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Rust library build
RUST_LIB_DIR = $(KERNEL_DIR)/rust_alloc
RUST_TARGET = $(OBJ_DIR)/librust_alloc.a

$(RUST_TARGET): $(RUST_LIB_DIR)/src/lib.rs | $(OBJ_DIR)
	cd $(RUST_LIB_DIR) && $(CARGO) build --release --target-dir ../../$(BUILD_DIR)
	cp $(BUILD_DIR)/release/librust_alloc.a $(RUST_TARGET)

# Create build directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/kernel/terminal
	mkdir -p $(OBJ_DIR)/kernel/fs
	mkdir -p $(OBJ_DIR)/kernel/window
	mkdir -p $(OBJ_DIR)/kernel/boot
	mkdir -p $(OBJ_DIR)/kernel/memory
	mkdir -p $(OBJ_DIR)/kernel/apps
	mkdir -p $(OBJ_DIR)/drivers/vga
	mkdir -p $(OBJ_DIR)/drivers/keyboard
	mkdir -p $(OBJ_DIR)/kernel/desktop
	mkdir -p $(OBJ_DIR)/drivers/network

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm | $(OBJ_DIR)
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Assemble top-level boot.S (NASM syntax)
$(OBJ_DIR)/bootS.o: $(SRC_DIR)/boot/boot.S | $(OBJ_DIR)
	$(AS) -f elf32 $< -o $@

# Assemble interrupt handlers
$(OBJ_DIR)/kernel/interrupts_asm.o: $(KERNEL_DIR)/interrupts.asm | $(OBJ_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Assemble process context switching
$(OBJ_DIR)/kernel/process_asm.o: $(KERNEL_DIR)/process.asm | $(OBJ_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Use top-level boot.S for multiboot header/entry
BOOT_ASM_SOURCES = boot.S
BOOT_ASM_OBJECTS = $(OBJ_DIR)/bootS.o
INTERRUPT_ASM_OBJECTS = $(OBJ_DIR)/kernel/interrupts_asm.o
PROCESS_ASM_OBJECTS = $(OBJ_DIR)/kernel/process_asm.o

# Link kernel (multiboot must be first!)
$(KERNEL_BIN): $(BOOT_ASM_OBJECTS) $(KERNEL_OBJECTS) $(DRIVER_OBJECTS) $(NET_OBJECTS) $(INTERRUPT_ASM_OBJECTS) $(PROCESS_ASM_OBJECTS) $(RUST_TARGET) | $(OBJ_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

# Run in QEMU (using ELF multiboot kernel)
run: $(KERNEL_BIN)
	qemu-system-x86_64 -kernel $(KERNEL_BIN) -m 128M

# Debug in QEMU (using ELF multiboot kernel)
debug: $(KERNEL_BIN)
	qemu-system-x86_64 -kernel $(KERNEL_BIN) -m 128M -s -S

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y build-essential nasm grub-pc-bin grub-common xorriso qemu-system-x86

# Help
help:
	@echo "CompileOS Build System"
	@echo "Available targets:"
	@echo "  all          - Build kernel (default)"
	@echo "  clean        - Clean build artifacts"
	@echo "  run          - Run in QEMU"
	@echo "  debug        - Run in QEMU with GDB server"
	@echo "  install-deps - Install build dependencies"
	@echo "  help         - Show this help"

.PHONY: all clean run debug install-deps help
