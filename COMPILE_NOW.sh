#!/bin/bash
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS

echo "Cleaning and rebuilding..."
make clean
make 2>&1 | tail -20

if [ -f build/kernel.bin ]; then
    echo "✓ Build successful!"
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
    grub-mkrescue -o compileos.iso iso 2>&1 | grep -v warning || true
    echo "✓ Booting..."
    qemu-system-x86_64 -cdrom compileos.iso -m 512M
else
    echo "✗ Build failed - check errors above"
fi
