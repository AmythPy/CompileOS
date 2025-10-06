#!/bin/bash
# WORKING SOLUTION - Gets CompileOS running NOW

set -e

echo "========================================"
echo "CompileOS - WORKING BOOT SOLUTION"
echo "========================================"
echo ""

cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

# Step 1: Build kernel
echo "[1/3] Building kernel..."
make clean
make
echo "✓ Kernel built"
echo ""

# Step 2: Use QEMU's direct kernel loading (bypass multiboot)
echo "[2/3] Testing with direct kernel load..."
echo ""
echo "Starting QEMU with direct kernel loading..."
echo "This bypasses GRUB and loads kernel directly."
echo ""

# Use QEMU's -kernel flag with proper options
qemu-system-x86_64 \
    -kernel build/kernel.bin \
    -m 512M \
    -serial stdio \
    -no-reboot \
    -no-shutdown \
    -d int,cpu_reset \
    -D qemu.log

echo ""
echo "If that didn't work, check qemu.log for errors"
