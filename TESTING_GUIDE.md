# CompileOS Testing Guide 🧪

## Prerequisites Installation

Before building CompileOS, you need to install the required tools:

### Required Tools
1. **GCC (MinGW-w64)** - C compiler
2. **NASM** - Assembler
3. **LD** - Linker (comes with MinGW)
4. **QEMU** (optional) - For testing

---

## Step 1: Install Build Tools 🛠️

### Option A: Install MinGW-w64 (Recommended)

1. **Download MinGW-w64:**
   - Visit: https://www.mingw-w64.org/downloads/
   - Or use: https://github.com/niXman/mingw-builds-binaries/releases
   - Download: `x86_64-posix-seh` version (latest)

2. **Extract and Add to PATH:**
   ```cmd
   # Extract to C:\mingw64
   # Add to PATH: C:\mingw64\bin
   ```

3. **Verify Installation:**
   ```cmd
   gcc --version
   ld --version
   ```

### Option B: Install MSYS2 (Alternative)

1. **Download MSYS2:**
   - Visit: https://www.msys2.org/
   - Download and install

2. **Install tools:**
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S nasm
   ```

3. **Add to PATH:**
   ```
   C:\msys64\mingw64\bin
   ```

### Install NASM

1. **Download NASM:**
   - Visit: https://www.nasm.us/
   - Download Windows installer

2. **Install and add to PATH:**
   ```
   C:\Program Files\NASM
   ```

3. **Verify:**
   ```cmd
   nasm -v
   ```

### Install QEMU (Optional, for testing)

1. **Download QEMU:**
   - Visit: https://www.qemu.org/download/#windows
   - Download Windows installer

2. **Install and add to PATH:**
   ```
   C:\Program Files\qemu
   ```

3. **Verify:**
   ```cmd
   qemu-system-x86_64 --version
   ```

---

## Step 2: Build CompileOS 🔨

Once tools are installed:

### Quick Build
```cmd
cd c:\Users\ccoll\OneDrive\Desktop\CompileOS
build_test.bat
```

### Manual Build (if script fails)

```cmd
# Create directories
mkdir build
mkdir build\obj
mkdir build\obj\kernel
mkdir build\obj\kernel\memory
mkdir build\obj\kernel\process
mkdir build\obj\kernel\terminal
mkdir build\obj\kernel\repl
mkdir build\obj\kernel\debugger
mkdir build\obj\kernel\desktop
mkdir build\obj\kernel\fs
mkdir build\obj\hal
mkdir build\obj\hal\arch\x86_64
mkdir build\obj\drivers\vga
mkdir build\obj\drivers\keyboard
mkdir build\obj\boot

# Set compiler flags
set CFLAGS=-Wall -Wextra -std=c99 -ffreestanding -fno-builtin -fno-stack-protector -mno-red-zone -mcmodel=kernel -fno-pic -fno-pie -Isrc -Isrc/hal

# Compile kernel sources
gcc %CFLAGS% -c src/kernel/main.c -o build/obj/kernel/main.o
gcc %CFLAGS% -c src/kernel/util.c -o build/obj/kernel/util.o
gcc %CFLAGS% -c src/kernel/memory/memory.c -o build/obj/kernel/memory/memory.o
gcc %CFLAGS% -c src/kernel/memory/multibit.c -o build/obj/kernel/memory/multibit.o
gcc %CFLAGS% -c src/kernel/memory/memory_tools.c -o build/obj/kernel/memory/memory_tools.o
gcc %CFLAGS% -c src/kernel/process/process.c -o build/obj/kernel/process/process.o
gcc %CFLAGS% -c src/kernel/terminal/terminal.c -o build/obj/kernel/terminal/terminal.o
gcc %CFLAGS% -c src/kernel/repl/repl.c -o build/obj/kernel/repl/repl.o
gcc %CFLAGS% -c src/kernel/debugger/debugger.c -o build/obj/kernel/debugger/debugger.o
gcc %CFLAGS% -c src/kernel/desktop/desktop.c -o build/obj/kernel/desktop/desktop.o
gcc %CFLAGS% -c src/kernel/fs/fs.c -o build/obj/kernel/fs/fs.o
gcc %CFLAGS% -c src/hal/hal.c -o build/obj/hal/hal.o
gcc %CFLAGS% -c src/hal/arch/x86_64/cpu.c -o build/obj/hal/arch/x86_64/cpu.o
gcc %CFLAGS% -c src/hal/arch/x86_64/interrupts.c -o build/obj/hal/arch/x86_64/interrupts.o
gcc %CFLAGS% -c src/drivers/vga/vga.c -o build/obj/drivers/vga/vga.o
gcc %CFLAGS% -c src/drivers/keyboard/keyboard.c -o build/obj/drivers/keyboard/keyboard.o

# Assemble
nasm -f elf64 src/hal/arch/x86_64/interrupts.asm -o build/obj/hal/arch/x86_64/interrupts_asm.o
nasm -f elf64 src/boot/boot.asm -o build/obj/boot/boot.o

# Link
ld -T linker.ld -nostdlib -o build/kernel.bin ^
    build/obj/kernel/main.o ^
    build/obj/kernel/util.o ^
    build/obj/kernel/memory/memory.o ^
    build/obj/kernel/memory/multibit.o ^
    build/obj/kernel/memory/memory_tools.o ^
    build/obj/kernel/process/process.o ^
    build/obj/kernel/terminal/terminal.o ^
    build/obj/kernel/repl/repl.o ^
    build/obj/kernel/debugger/debugger.o ^
    build/obj/kernel/desktop/desktop.o ^
    build/obj/kernel/fs/fs.o ^
    build/obj/hal/hal.o ^
    build/obj/hal/arch/x86_64/cpu.o ^
    build/obj/hal/arch/x86_64/interrupts.o ^
    build/obj/hal/arch/x86_64/interrupts_asm.o ^
    build/obj/drivers/vga/vga.o ^
    build/obj/drivers/keyboard/keyboard.o
```

---

## Step 3: Run CompileOS 🎮

### Option A: Run in QEMU (Recommended)

```cmd
qemu-system-x86_64 -kernel build/kernel.bin -m 512M
```

**What you should see:**
1. Blue desktop background
2. Title bar: "CompileOS v0.1.0"
3. Main terminal window with welcome message
4. Status bar at bottom with function keys
5. Prompt: `CompileOS>`

### Option B: Create Bootable ISO

If you have GRUB tools:

```cmd
# Create ISO structure
mkdir iso
mkdir iso\boot
mkdir iso\boot\grub

# Copy kernel
copy build\kernel.bin iso\boot\kernel.bin

# Create grub.cfg
echo set timeout=0 > iso\boot\grub\grub.cfg
echo set default=0 >> iso\boot\grub\grub.cfg
echo menuentry "CompileOS" { >> iso\boot\grub\grub.cfg
echo     multiboot /boot/kernel.bin >> iso\boot\grub\grub.cfg
echo     boot >> iso\boot\grub\grub.cfg
echo } >> iso\boot\grub\grub.cfg

# Create ISO
grub-mkrescue -o compileos.iso iso
```

Then boot in VirtualBox or VMware.

### Option C: Test Without QEMU

If you don't have QEMU, you can:
1. Create a bootable USB drive
2. Use VirtualBox/VMware
3. Use Bochs emulator

---

## Step 4: Test Features 🧪

Once CompileOS boots, try these tests:

### Test 1: Terminal Commands

Type these commands:

```
CompileOS> help
```
**Expected:** List of all available commands

```
CompileOS> echo Hello World!
```
**Expected:** `Hello World!`

```
CompileOS> version
```
**Expected:** CompileOS version info

```
CompileOS> memory
```
**Expected:** Memory statistics

```
CompileOS> history
```
**Expected:** List of commands you've typed

```
CompileOS> clear
```
**Expected:** Screen clears

### Test 2: REPL Expression Evaluator

```
CompileOS> calc 2 + 3
```
**Expected:** `Result: 5`

```
CompileOS> calc (10 + 5) * 2
```
**Expected:** `Result: 30`

```
CompileOS> calc x = 100
```
**Expected:** `Result: 100`

```
CompileOS> calc x * 2
```
**Expected:** `Result: 200`

```
CompileOS> calc y = x + 50
```
**Expected:** `Result: 150`

### Test 3: Filesystem (if you add file commands)

```
CompileOS> touch test.txt
CompileOS> write test.txt Hello from CompileOS!
CompileOS> cat test.txt
CompileOS> ls
```

### Test 4: Keyboard Input

- **Type characters** - Should appear on screen
- **Press Backspace** - Should delete characters
- **Press Enter** - Should execute command
- **Type long lines** - Should handle properly

### Test 5: Desktop Features

- **Window borders** - Should be visible
- **Title bar** - Should show OS name
- **Status bar** - Should show function keys
- **Colors** - Should be blue background, colored text
- **Focus** - Main window should have yellow border

---

## Troubleshooting 🔧

### Build Errors

**"gcc: command not found"**
- Install MinGW-w64
- Add to PATH
- Restart terminal

**"nasm: command not found"**
- Install NASM
- Add to PATH
- Restart terminal

**"undefined reference to..."**
- Check all source files are compiled
- Verify linking order
- Check for typos in function names

**"multiple definition of..."**
- Remove duplicate implementations
- Check for conflicting symbols

### Runtime Errors

**Black screen**
- Check VGA initialization
- Verify desktop_start() is called
- Check QEMU command

**No keyboard input**
- Verify keyboard_init() is called
- Check IRQ 1 is enabled
- Ensure interrupts are enabled

**Garbled display**
- Check VGA mode (should be text 80x25)
- Verify color values
- Check buffer writes

**Crashes/reboots**
- Check for null pointer dereferences
- Verify stack is set up correctly
- Check interrupt handlers

---

## Expected Output 📺

When CompileOS boots successfully, you should see:

```
┌────────────────────────────────────────────────────────────────────────────┐
│ CompileOS v0.1.0 - Hardware Agnostic Development Platform            [X]  │
├────────────────────────────────────────────────────────────────────────────┤
│                                                                            │
│  ┌──────────────────────────────────────────────────────────────────────┐ │
│  │ CompileOS Terminal - Interactive Shell                              │ │
│  ├──────────────────────────────────────────────────────────────────────┤ │
│  │                                                                      │ │
│  │  Welcome to CompileOS Interactive Desktop!                          │ │
│  │                                                                      │ │
│  │  Features:                                                           │ │
│  │    * PS/2 Keyboard Input                                            │ │
│  │    * Interactive Terminal Shell                                     │ │
│  │    * REPL Expression Evaluator                                      │ │
│  │    * In-Memory Filesystem                                           │ │
│  │    * Window Management                                              │ │
│  │                                                                      │ │
│  │  Type 'help' for available commands                                 │ │
│  │                                                                      │ │
│  │  System Info:                                                        │ │
│  │    Architecture: x86_64                                             │ │
│  │    Memory: 16/32/64-bit support                                     │ │
│  │    Status: Ready                                                     │ │
│  │                                                                      │ │
│  │  CompileOS> _                                                        │ │
│  └──────────────────────────────────────────────────────────────────────┘ │
├────────────────────────────────────────────────────────────────────────────┤
│ F1-Help | F2-Files | F3-REPL | F4-Memory | F5-Debug | F10-Exit  Status: Ready │
└────────────────────────────────────────────────────────────────────────────┘
```

---

## Quick Start Checklist ✅

- [ ] Install MinGW-w64 (GCC)
- [ ] Install NASM
- [ ] Install QEMU (optional)
- [ ] Add tools to PATH
- [ ] Restart terminal
- [ ] Run `build_test.bat`
- [ ] Check for `build/kernel.bin`
- [ ] Run `qemu-system-x86_64 -kernel build/kernel.bin -m 512M`
- [ ] See desktop appear
- [ ] Type `help` command
- [ ] Test other commands
- [ ] Celebrate! 🎉

---

## Next Steps After Testing 🚀

Once everything works:

1. **Add more commands** - Implement `ls`, `cat`, `rm`, `cp`
2. **Enhance REPL** - Add more operators, functions
3. **Improve desktop** - Add more windows, themes
4. **Add persistence** - Save files to disk
5. **Build apps** - Create calculator, text editor
6. **Share it!** - Show off your OS to others

---

## Getting Help 💬

If you encounter issues:

1. Check error messages carefully
2. Verify all prerequisites are installed
3. Try manual build commands
4. Check documentation files
5. Review source code for typos

---

## Success Criteria ✨

You'll know it's working when:

- ✅ Build completes without errors
- ✅ QEMU window opens
- ✅ Desktop appears with blue background
- ✅ You can type characters
- ✅ Commands execute and show results
- ✅ Calculator works (2+3=5)
- ✅ You feel like an OS developer! 😎

---

**Good luck testing CompileOS!** 🚀🖥️✨

Your operating system is ready to run - just install the tools and build it!
