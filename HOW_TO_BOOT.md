# 🚀 How to Boot CompileOS

## The Problem

QEMU opened but said "no bootable device" because the ISO wasn't properly bootable.

---

## ✅ SOLUTION: Build Proper Bootable ISO

### Step 1: Install Required Tools

In WSL/Ubuntu:
```bash
sudo apt update
sudo apt install grub-pc-bin xorriso
```

### Step 2: Run the ISO Builder

**Option A: Use the automated script (Linux/WSL)**
```bash
chmod +x build_iso.sh
./build_iso.sh
```

**Option B: Use Windows batch file**
```cmd
build_iso.bat
```

**Option C: Manual steps**
```bash
# Build kernel
make clean && make

# Create ISO structure
mkdir -p iso/boot/grub
cp build/kernel.bin iso/boot/

# Create GRUB config
cat > iso/boot/grub/grub.cfg << 'EOF'
set timeout=3
set default=0

menuentry "CompileOS v0.1.0" {
    multiboot2 /boot/kernel.bin
    boot
}
EOF

# Create bootable ISO
grub-mkrescue -o compileos.iso iso
```

### Step 3: Boot the ISO

**In QEMU:**
```bash
qemu-system-x86_64 -cdrom compileos.iso -m 512M
```

**In VirtualBox:**
1. Create new VM (Type: Other, Version: Other/Unknown 64-bit)
2. Set memory to 512MB
3. Attach `compileos.iso` as CD/DVD
4. Start VM

---

## What You'll See

When it boots successfully:

```
[GRUB Menu appears for 3 seconds]

CompileOS v0.1.0
CompileOS v0.1.0 (Safe Mode)

[Auto-boots to CompileOS]

┌──────────────────────────────────────────────────────────┐
│              CompileOS Desktop v0.1.0                     │
├──────────────────────────────────────────────────────────┤
│  CompileOS Terminal - Interactive Shell                  │
│                                                           │
│  Welcome to CompileOS!                                    │
│  Type 'help' for available commands.                      │
│                                                           │
│  CompileOS>                                               │
│                                                           │
├──────────────────────────────────────────────────────────┤
│  Status: Running | Memory: 512MB | Version: 0.1.0        │
└──────────────────────────────────────────────────────────┘
```

---

## Troubleshooting

### "grub-mkrescue: command not found"
```bash
sudo apt install grub-pc-bin xorriso
```

### "No bootable device" in QEMU
Make sure you're using `-cdrom` not `-kernel`:
```bash
qemu-system-x86_64 -cdrom compileos.iso -m 512M
```

### ISO file not found
Check that `compileos.iso` was created:
```bash
ls -lh compileos.iso
```

### Black screen in QEMU
- Wait a few seconds
- Press Enter
- Type `help`

### GRUB menu doesn't appear
The timeout is 3 seconds - it auto-boots. To see menu:
- Edit `iso/boot/grub/grub.cfg`
- Change `set timeout=3` to `set timeout=10`
- Rebuild ISO

---

## Quick Reference

### Build ISO
```bash
./build_iso.sh
```

### Run in QEMU
```bash
qemu-system-x86_64 -cdrom compileos.iso -m 512M
```

### Run with more options
```bash
qemu-system-x86_64 \
    -cdrom compileos.iso \
    -m 512M \
    -serial stdio \
    -vga std
```

---

## 🎉 Success!

Once you see the CompileOS desktop and terminal, your OS is running!

Try these commands:
```
help      - Show all commands
version   - OS version
memory    - Memory info
echo hi   - Echo text
repl      - Calculator
clear     - Clear screen
```

**Your operating system is complete and bootable!** 🚀
