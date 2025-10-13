# CompileOS QEMU Launcher Script
# PowerShell version with better error handling

Write-Host "CompileOS v0.1.0 - Hardware Agnostic Development Platform" -ForegroundColor Cyan
Write-Host "=========================================================" -ForegroundColor Cyan
Write-Host ""

# Check if kernel exists
if (-not (Test-Path "build\kernel.bin")) {
    Write-Host "ERROR: Kernel binary not found at build\kernel.bin" -ForegroundColor Red
    Write-Host "Please run 'make' or build the kernel first." -ForegroundColor Yellow
    exit 1
}

# Check QEMU installation
$qemuPath = "C:\Program Files\qemu\qemu-system-x86_64.exe"
if (-not (Test-Path $qemuPath)) {
    Write-Host "ERROR: QEMU not found at $qemuPath" -ForegroundColor Red
    Write-Host "Please install QEMU first." -ForegroundColor Yellow
    exit 1
}

Write-Host "Kernel: build\kernel.bin" -ForegroundColor Green
Write-Host "QEMU: $qemuPath" -ForegroundColor Green
Write-Host "Memory: 512MB" -ForegroundColor Green
Write-Host ""

Write-Host "QEMU Controls:" -ForegroundColor Yellow
Write-Host "  Ctrl+Alt+G  - Release mouse from QEMU window" -ForegroundColor Gray
Write-Host "  Ctrl+Alt+F  - Toggle fullscreen" -ForegroundColor Gray
Write-Host "  Ctrl+Alt+Q  - Quit QEMU" -ForegroundColor Gray
Write-Host ""

Write-Host "Starting CompileOS..." -ForegroundColor Green
Write-Host ""

# Launch QEMU with CompileOS kernel
& $qemuPath -kernel build\kernel.bin -m 512M -serial stdio -monitor stdio

Write-Host ""
Write-Host "CompileOS session ended." -ForegroundColor Cyan
