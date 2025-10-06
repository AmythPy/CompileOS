#!/bin/bash
# CompileOS ISO Builder
# Creates a bootable ISO with GRUB

set -e  # Exit on error

echo "=========================================="
echo "CompileOS ISO Builder"
echo "=========================================="
echo ""

# Step 1: Build kernel
echo "[1/5] Building kernel..."
make clean
make
echo "✓ Kernel built successfully"
echo ""

# Step 2: Create ISO directory structure
echo "[2/5] Creating ISO structure..."
rm -rf iso
mkdir -p iso/boot/grub
echo "✓ ISO directories created"
echo ""

# Step 3: Copy kernel
echo "[3/5] Copying kernel..."
cp build/kernel.bin iso/boot/kernel.bin
echo "✓ Kernel copied to iso/boot/"
echo ""

# Step 4: Create GRUB configuration
echo "[4/5] Creating GRUB configuration..."
cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=3
set default=0

menuentry "CompileOS v0.1.0" {
    multiboot2 /boot/kernel.bin
    boot
}

menuentry "CompileOS v0.1.0 (Safe Mode)" {
    multiboot2 /boot/kernel.bin --safe
    boot
}
EOF
echo "✓ GRUB config created"
echo ""

# Step 5: Create bootable ISO
echo "[5/5] Creating bootable ISO..."
if command -v grub-mkrescue &> /dev/null; then
    grub-mkrescue -o compileos.iso iso
    echo "✓ ISO created: compileos.iso"
else
    echo "✗ grub-mkrescue not found!"
    echo ""
    echo "Please install GRUB tools:"
    echo "  sudo apt install grub-pc-bin xorriso"
    exit 1
fi

echo ""
echo "=========================================="
echo "Build Complete!"
echo "=========================================="
echo ""
echo "ISO file: compileos.iso"
echo "Size: $(du -h compileos.iso | cut -f1)"
echo ""
echo "To run in QEMU:"
echo "  qemu-system-x86_64 -cdrom compileos.iso -m 512M"
echo ""
echo "To run in VirtualBox:"
echo "  1. Create new VM (Type: Other, Version: Other/Unknown 64-bit)"
echo "  2. Attach compileos.iso as CD/DVD"
echo "  3. Boot VM"
echo ""
