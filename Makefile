# CompileOS Makefile
# Builds the CompileOS kernel and bootloader

# Compiler and tools
CC = gcc
AS = nasm
LD = ld
OBJCOPY = objcopy

# Architecture
ARCH = x86_64

# Compiler flags
CFLAGS := -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal
ASFLAGS = -f elf64
LDFLAGS = -T linker.ld -nostdlib

# Directories
SRC_DIR = src
KERNEL_DIR = $(SRC_DIR)/kernel
HAL_DIR = $(SRC_DIR)/hal
BOOT_DIR = $(SRC_DIR)/boot
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
KERNEL_SOURCES = $(KERNEL_DIR)/main.c
KERNEL_SOURCES = $(wildcard $(KERNEL_DIR)/*.c) $(wildcard $(KERNEL_DIR)/*/*.c)
HAL_SOURCES = $(wildcard $(HAL_DIR)/*.c) $(wildcard $(HAL_DIR)/arch/*/*.c)
HAL_ASM_SOURCES = $(wildcard $(HAL_DIR)/arch/*/*.asm)
BOOT_SOURCES = $(wildcard $(BOOT_DIR)/*.asm)

# Object files
KERNEL_OBJECTS = $(KERNEL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
HAL_OBJECTS = $(HAL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
HAL_ASM_OBJECTS = $(HAL_ASM_SOURCES:$(SRC_DIR)/%.asm=$(OBJ_DIR)/%.o)
BOOT_OBJECTS = $(BOOT_SOURCES:$(SRC_DIR)/%.asm=$(OBJ_DIR)/%.o)

# Target files
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
BOOTLOADER_BIN = $(BUILD_DIR)/bootloader.bin
EMBEDDED_BOOTLOADER_BIN = $(BUILD_DIR)/embedded_bootloader.bin
EMBEDDED_OS_BMP = $(BUILD_DIR)/compileos_embedded.bmp
ISO_IMAGE = $(BUILD_DIR)/compileos.iso
EMBED_TOOL = $(BUILD_DIR)/embed_os

# Default target
all: $(ISO_IMAGE)

# Create build directories
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)/kernel/memory
	mkdir -p $(OBJ_DIR)/kernel/process
	mkdir -p $(OBJ_DIR)/kernel/debugger
	mkdir -p $(OBJ_DIR)/kernel/terminal
	mkdir -p $(OBJ_DIR)/kernel/repl
	mkdir -p $(OBJ_DIR)/hal/arch/x86_64
	mkdir -p $(OBJ_DIR)/hal/arch/arm64
	mkdir -p $(OBJ_DIR)/drivers/vga
	mkdir -p $(OBJ_DIR)/drivers/keyboard
	mkdir -p $(OBJ_DIR)/boot
	mkdir -p $(OBJ_DIR)/tools

# Compile C sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble ASM sources
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm | $(OBJ_DIR)
	$(AS) $(ASFLAGS) $< -o $@

# Link kernel
$(KERNEL_BIN): $(KERNEL_OBJECTS) $(HAL_OBJECTS) $(HAL_ASM_OBJECTS) | $(OBJ_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

# Create bootloader binary
$(BOOTLOADER_BIN): $(BOOT_OBJECTS) | $(OBJ_DIR)
	$(OBJCOPY) -O binary $< $@

# Create embedded bootloader binary
$(EMBEDDED_BOOTLOADER_BIN): $(OBJ_DIR)/boot/embedded_boot.o | $(OBJ_DIR)
	$(OBJCOPY) -O binary $< $@

# Create embed tool
$(EMBED_TOOL): $(OBJ_DIR)/tools/embed_os.o | $(OBJ_DIR)
	$(CC) -o $@ $^

# Create embedded OS BMP
$(EMBEDDED_OS_BMP): $(KERNEL_BIN) $(EMBED_TOOL) | $(BUILD_DIR)
	$(EMBED_TOOL) embed splash.bmp $(KERNEL_BIN) $@

# Create ISO image
$(ISO_IMAGE): $(KERNEL_BIN) $(BOOTLOADER_BIN)
	mkdir -p $(BUILD_DIR)/iso/boot/grub
	cp $(KERNEL_BIN) $(BUILD_DIR)/iso/boot/
	cp $(BOOTLOADER_BIN) $(BUILD_DIR)/iso/boot/
	cp grub.cfg $(BUILD_DIR)/iso/boot/grub/
	grub-mkrescue -o $@ $(BUILD_DIR)/iso/

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Run in QEMU
run: $(ISO_IMAGE)
	qemu-system-x86_64 -cdrom $(ISO_IMAGE) -m 512M

# Debug in QEMU
debug: $(ISO_IMAGE)
	qemu-system-x86_64 -cdrom $(ISO_IMAGE) -m 512M -s -S

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
build/obj/kernel/util.o: src/kernel/util.c
	gcc  -c src/kernel/util.c -o build/obj/kernel/util.o

build/obj/hal/arch/x86_64/isr.o: src/hal/arch/x86_64/isr.S
	nasm -f elf64 src/hal/arch/x86_64/isr.S -o build/obj/hal/arch/x86_64/isr.o
build/obj/kernel/util.o: src/kernel/util.c
	gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/util.c -o build/obj/kernel/util.o

build/obj/hal/arch/x86_64/isr.o: src/hal/arch/x86_64/isr.S
	nasm -f elf64 src/hal/arch/x86_64/isr.S -o build/obj/hal/arch/x86_64/isr.o
build/obj/kernel/util.o: src/kernel/util.c
	gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/util.c -o build/obj/kernel/util.o

build/obj/hal/arch/x86_64/isr.o: src/hal/arch/x86_64/isr.S
	nasm -f elf64 src/hal/arch/x86_64/isr.S -o build/obj/hal/arch/x86_64/isr.o
build/obj/kernel/util.o: src/kernel/util.c
	gcc -Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal -c src/kernel/util.c -o build/obj/kernel/util.o

build/obj/hal/arch/x86_64/isr.o: src/hal/arch/x86_64/isr.S
	nasm -f elf64 src/hal/arch/x86_64/isr.S -o build/obj/hal/arch/x86_64/isr.o
