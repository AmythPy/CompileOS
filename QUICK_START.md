# CompileOS Quick Start Guide ⚡

## 🎯 Goal
Get CompileOS running in **5 simple steps**!

---

## Step 1: Install MinGW-w64 (GCC Compiler) 📥

### Easiest Method:
1. Download: https://github.com/niXman/mingw-builds-binaries/releases
2. Get: `x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev0.7z`
3. Extract to: `C:\mingw64`
4. Add to PATH:
   - Open: System Properties → Environment Variables
   - Edit: Path
   - Add: `C:\mingw64\bin`
   - Click OK

### Verify:
```cmd
gcc --version
```
Should show: `gcc (x86_64-posix-seh-rev0, Built by MinGW-W64 project)`

---

## Step 2: Install NASM (Assembler) 📥

1. Download: https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/win64/nasm-2.16.01-installer-x64.exe
2. Run installer
3. Add to PATH: `C:\Program Files\NASM`

### Verify:
```cmd
nasm -v
```
Should show: `NASM version 2.16.01`

---

## Step 3: Install QEMU (Emulator) 📥

1. Download: https://qemu.weilnetz.de/w64/
2. Get latest: `qemu-w64-setup-*.exe`
3. Run installer
4. Add to PATH: `C:\Program Files\qemu`

### Verify:
```cmd
qemu-system-x86_64 --version
```
Should show: `QEMU emulator version`

---

## Step 4: Build CompileOS 🔨

```cmd
cd c:\Users\ccoll\OneDrive\Desktop\CompileOS
build_test.bat
```

### Expected Output:
```
========================================
CompileOS Build Test
========================================

Checking for required tools...
Tools found: GCC and NASM

Creating build directories...
Directories created.

Compiling kernel sources...
[... compilation messages ...]

Linking kernel...

========================================
Build completed successfully!
========================================
Kernel binary: build\kernel.bin
```

---

## Step 5: Run CompileOS! 🚀

```cmd
qemu-system-x86_64 -kernel build\kernel.bin -m 512M
```

### You Should See:
- 🖥️ Blue desktop background
- 📝 Welcome message
- ⌨️ Prompt: `CompileOS>`
- 🎨 Colored windows and borders

---

## Try These Commands! 💻

Once booted, type:

```
help
```
See all available commands

```
echo Hello World!
```
Test basic output

```
calc 2 + 3
```
Test calculator (should show: Result: 5)

```
calc x = 100
```
Set a variable

```
calc x * 2
```
Use the variable (should show: Result: 200)

```
memory
```
See memory statistics

```
version
```
See OS version

```
history
```
See command history

---

## Troubleshooting 🔧

### "gcc: command not found"
- Restart your terminal/command prompt
- Check PATH includes `C:\mingw64\bin`
- Run: `echo %PATH%` to verify

### "nasm: command not found"
- Restart your terminal/command prompt
- Check PATH includes NASM directory
- Run: `where nasm` to find it

### Build fails with errors
- Make sure you're in the CompileOS directory
- Check that all source files exist
- Try running commands manually from TESTING_GUIDE.md

### QEMU doesn't start
- Check QEMU is installed
- Try: `qemu-system-x86_64 --help`
- Make sure `build\kernel.bin` exists

### Black screen in QEMU
- This might be normal during boot
- Wait a few seconds
- Press Ctrl+C to exit and rebuild

---

## Alternative: Use WSL (Windows Subsystem for Linux) 🐧

If you prefer Linux tools:

```bash
# Install WSL
wsl --install

# In WSL, install tools
sudo apt update
sudo apt install build-essential nasm qemu-system-x86

# Build
cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS
make

# Run
qemu-system-x86_64 -kernel build/kernel.bin -m 512M
```

---

## What's Next? 🎯

After you see CompileOS running:

1. ✅ **Test all commands** - Try help, echo, calc, memory
2. ✅ **Test calculator** - Try: `calc (10+5)*2`
3. ✅ **Test variables** - Try: `calc x=50`, then `calc x*3`
4. ✅ **Add file commands** - Implement ls, cat, touch
5. ✅ **Customize desktop** - Change colors, add windows
6. ✅ **Build your app** - Use the OS as a platform!

---

## Quick Reference Card 📋

### Build Commands
```cmd
build_test.bat              # Build everything
```

### Run Commands
```cmd
qemu-system-x86_64 -kernel build\kernel.bin -m 512M
```

### QEMU Shortcuts
- `Ctrl+Alt+G` - Release mouse
- `Ctrl+Alt+F` - Toggle fullscreen
- `Ctrl+C` - Exit QEMU

### Terminal Commands
- `help` - Show commands
- `clear` - Clear screen
- `echo <text>` - Print text
- `calc <expr>` - Evaluate expression
- `memory` - Show memory stats
- `version` - Show OS version
- `history` - Show command history

---

## Success! 🎉

When you see this, you've successfully built and run your own operating system:

```
CompileOS> help

Available commands:
  help         - Show available commands
  clear        - Clear the screen
  echo         - Echo arguments to screen
  memory       - Show memory statistics
  repl         - Start REPL mode
  history      - Show command history
  version      - Show OS version
  uptime       - Show system uptime

CompileOS> calc 2 + 3
Result: 5

CompileOS> 
```

**Congratulations! You're now running CompileOS!** 🎊🖥️✨

---

## Download Links Summary 🔗

1. **MinGW-w64:** https://github.com/niXman/mingw-builds-binaries/releases
2. **NASM:** https://www.nasm.us/pub/nasm/releasebuilds/
3. **QEMU:** https://qemu.weilnetz.de/w64/

---

**Total Setup Time: ~15 minutes**
**Total Build Time: ~30 seconds**
**Total Awesomeness: Infinite!** 😎
