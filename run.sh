#!/bin/bash
# CompileOS Run Script
# Alternative boot method using direct kernel loading

echo "Building CompileOS..."
make clean
make

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful!"
    echo "Starting QEMU..."
    echo ""
    
    # Try multiboot with explicit options
    qemu-system-x86_64 \
        -kernel build/kernel.bin \
        -m 512M \
        -cpu qemu64 \
        -no-reboot \
        -no-shutdown \
        -d guest_errors
else
    echo "Build failed!"
    exit 1
fi
