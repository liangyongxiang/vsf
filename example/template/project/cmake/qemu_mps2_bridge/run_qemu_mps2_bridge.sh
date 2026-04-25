#!/usr/bin/env sh
set -eu

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
QEMU_BIN=${QEMU_BIN:-qemu-system-arm}
BUILD_DIR=${BUILD_DIR:-"$SCRIPT_DIR/build-make"}
ELF=${ELF:-"$BUILD_DIR/vsf_qemu_mps2_bridge.elf"}
MACHINE=${MACHINE:-mps2-an500}

if [ ! -f "$ELF" ]; then
    echo "ELF not found: $ELF" >&2
    exit 1
fi

exec "$QEMU_BIN" \
    -M "$MACHINE" \
    -monitor none \
    -serial stdio \
    -nographic \
    -kernel "$ELF"
