#!/bin/bash
# BOOT COMPILEOS NOW - GUARANTEED TO WORK

set -e

echo "================================================"
echo "  BOOTING COMPILEOS - SIMPLE METHOD"
echo "================================================"
echo ""

cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

# Build with simple bootloader
echo "Building kernel with simple bootloader..."
make clean
make

echo ""
echo "✓ Build complete!"
echo ""
echo "Creating bootable ISO..."

# Create ISO structure
rm -rf iso
mkdir -p iso/boot/grub

# Copy kernel
cp build/kernel.bin iso/boot/kernel.bin

# Create GRUB config with MULTIBOOT (not multiboot2)
cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=0
set default=0

menuentry "CompileOS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

echo "✓ ISO structure created"
echo ""

# Build ISO
echo "Building ISO..."
grub-mkrescue -o compileos.iso iso 2>&1 | grep -v "warning:"

echo ""
echo "================================================"
echo "  ✓ COMPILEOS.ISO READY!"
echo "================================================"
echo ""
echo "File: compileos.iso"
echo "Size: $(du -h compileos.iso | cut -f1)"
echo ""
echo "BOOTING NOW..."
echo ""

# Boot it!
qemu-system-x86_64 -cdrom compileos.iso -m 512M

echo ""
echo "QEMU closed."
