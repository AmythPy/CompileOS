#!/bin/bash
set -e
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "Building CompileOS..."
make clean && make

echo "Creating ISO..."
rm -rf iso
mkdir -p iso/boot/grub
cp build/kernel.bin iso/boot/

cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=0
menuentry "CompileOS" {
    multiboot /boot/kernel.bin
    boot
}
EOF

grub-mkrescue -o compileos.iso iso 2>&1 | grep -v "warning:" || true

echo ""
echo "✓ Ready! Running..."
qemu-system-x86_64 -cdrom compileos.iso -m 512M
