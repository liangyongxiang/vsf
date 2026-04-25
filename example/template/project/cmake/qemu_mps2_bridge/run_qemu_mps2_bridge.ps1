param(
    [string]$QemuBin = $env:QEMU_BIN,
    [string]$BuildDir = (Join-Path $PSScriptRoot "build-make"),
    [string]$Elf = (Join-Path $BuildDir "vsf_qemu_mps2_bridge.elf"),
    [string]$Machine = "mps2-an500"
)

if (-not $QemuBin) {
    throw "Set QEMU_BIN to qemu-system-arm before running this script."
}

if (-not (Test-Path $QemuBin)) {
    throw "QEMU binary not found: $QemuBin"
}

if (-not (Test-Path $Elf)) {
    throw "ELF not found: $Elf"
}

& $QemuBin `
    -accel "tcg,tb-size=128" `
    -M $Machine `
    -monitor none `
    -serial stdio `
    -nographic `
    -kernel $Elf
