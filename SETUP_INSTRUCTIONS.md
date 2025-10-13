# 🛠️ CompileOS Setup Instructions

## You Need to Install Build Tools First!

I've detected that you don't have GCC installed yet. Here's exactly what to do:

---

## 📥 Step 1: Install MinGW-w64 (GCC Compiler)

### Option A: Quick Install (Recommended)

1. **Download the installer:**
   - Go to: https://github.com/niXman/mingw-builds-binaries/releases
   - Download: `x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev0.7z`
   - Or direct link: https://github.com/niXman/mingw-builds-binaries/releases/download/13.2.0-rt_v11-rev0/x86_64-13.2.0-release-posix-seh-ucrt-rt_v11-rev0.7z

2. **Extract the archive:**
   - Right-click the downloaded .7z file
   - Extract to `C:\mingw64`
   - You should have: `C:\mingw64\bin\gcc.exe`

3. **Add to PATH:**
   - Press `Windows + X` → System
   - Click "Advanced system settings"
   - Click "Environment Variables"
   - Under "System variables", find "Path"
   - Click "Edit"
   - Click "New"
   - Add: `C:\mingw64\bin`
   - Click OK on all dialogs

4. **Verify (IMPORTANT - Restart terminal first!):**
   - Open a NEW Command Prompt or PowerShell
   - Run: `gcc --version`
   - Should show: `gcc (x86_64-posix-seh-rev0, Built by MinGW-W64 project)`

### Option B: Use Chocolatey (If you have it)

```powershell
choco install mingw
```

### Option C: Use MSYS2

1. Download: https://www.msys2.org/
2. Install MSYS2
3. Open MSYS2 terminal
4. Run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
5. Add to PATH: `C:\msys64\mingw64\bin`

---

## 📥 Step 2: Install NASM (Assembler)

1. **Download:**
   - Go to: https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/win64/
   - Download: `nasm-2.16.01-installer-x64.exe`
   - Or direct link: https://www.nasm.us/pub/nasm/releasebuilds/2.16.01/win64/nasm-2.16.01-installer-x64.exe

2. **Install:**
   - Run the installer
   - Use default installation path: `C:\Program Files\NASM`

3. **Add to PATH:**
   - Same process as GCC
   - Add: `C:\Program Files\NASM`

4. **Verify (Restart terminal first!):**
   - Run: `nasm -v`
   - Should show: `NASM version 2.16.01`

---

## 📥 Step 3: Install QEMU (Emulator) - Optional but Recommended

1. **Download:**
   - Go to: https://qemu.weilnetz.de/w64/
   - Download latest: `qemu-w64-setup-*.exe`
   - Or try: https://qemu.weilnetz.de/w64/2023/qemu-w64-setup-20230424.exe

2. **Install:**
   - Run the installer
   - Use default path: `C:\Program Files\qemu`

3. **Add to PATH:**
   - Add: `C:\Program Files\qemu`

4. **Verify (Restart terminal first!):**
   - Run: `qemu-system-x86_64 --version`
   - Should show: `QEMU emulator version`

---

## ⚠️ IMPORTANT: Restart Your Terminal!

After installing and adding to PATH, you MUST:
1. Close ALL Command Prompt/PowerShell windows
2. Open a NEW terminal
3. Then verify the tools are found

---

## ✅ Verification Checklist

Open a NEW terminal and run these commands:

```cmd
gcc --version
```
✅ Should show GCC version

```cmd
ld --version
```
✅ Should show GNU ld version

```cmd
nasm -v
```
✅ Should show NASM version

```cmd
qemu-system-x86_64 --version
```
✅ Should show QEMU version (optional)

---

## 🚀 Once Tools Are Installed

After you have GCC and NASM installed and verified:

1. **Open a NEW terminal** (important!)

2. **Navigate to CompileOS:**
   ```cmd
   cd c:\Users\ccoll\OneDrive\Desktop\CompileOS
   ```

3. **Run the build script:**
   ```cmd
   build_test.bat
   ```

4. **If successful, run CompileOS:**
   ```cmd
   qemu-system-x86_64 -kernel build\kernel.bin -m 512M
   ```

---

## 🆘 Troubleshooting

### "gcc is not recognized"
- Did you add `C:\mingw64\bin` to PATH?
- Did you restart your terminal?
- Check PATH: `echo %PATH%`
- Look for `C:\mingw64\bin` in the output

### "nasm is not recognized"
- Did you add `C:\Program Files\NASM` to PATH?
- Did you restart your terminal?
- Try: `where nasm` to find it

### "Can't extract .7z file"
- Download 7-Zip: https://www.7-zip.org/
- Or use WinRAR
- Or download the .zip version instead

### Still having issues?
- Try the MSYS2 method instead
- Or use WSL (Windows Subsystem for Linux)
- Check that you have admin rights

---

## 🐧 Alternative: Use WSL (Windows Subsystem for Linux)

If Windows tools are problematic, use WSL:

1. **Install WSL:**
   ```powershell
   wsl --install
   ```

2. **Restart computer**

3. **Open Ubuntu (WSL):**
   ```bash
   sudo apt update
   sudo apt install build-essential nasm qemu-system-x86
   ```

4. **Navigate to CompileOS:**
   ```bash
   cd /mnt/c/Users/ccoll/OneDrive/Desktop/CompileOS
   ```

5. **Build:**
   ```bash
   make
   ```

6. **Run:**
   ```bash
   qemu-system-x86_64 -kernel build/kernel.bin -m 512M
   ```

---

## 📋 Quick Reference

| Tool | Purpose | Download Link |
|------|---------|---------------|
| MinGW-w64 | C Compiler (GCC) | https://github.com/niXman/mingw-builds-binaries/releases |
| NASM | Assembler | https://www.nasm.us/pub/nasm/releasebuilds/ |
| QEMU | Emulator | https://qemu.weilnetz.de/w64/ |
| 7-Zip | Extract .7z files | https://www.7-zip.org/ |

---

## ⏱️ Estimated Time

- Download tools: 5-10 minutes
- Install tools: 5 minutes
- Add to PATH: 2 minutes
- Verify: 1 minute
- **Total: ~15-20 minutes**

Then building CompileOS takes ~30 seconds!

---

## 🎯 Next Steps

1. ✅ Install MinGW-w64 (GCC)
2. ✅ Install NASM
3. ✅ Install QEMU (optional)
4. ✅ Add all to PATH
5. ✅ Restart terminal
6. ✅ Verify with `gcc --version` and `nasm -v`
7. ✅ Run `build_test.bat`
8. ✅ Run CompileOS in QEMU!

---

**Once you have the tools installed, I can help you build and run CompileOS immediately!** 🚀

Let me know when you've installed GCC and NASM, and I'll help you build it!
