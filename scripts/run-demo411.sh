#!/bin/sh
# Run this project's firmware on a QEMU build with blackpill-f411ce support.
set -eu
script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
project_dir=$(CDPATH= cd -- "$script_dir/.." && pwd)
firmware=${1:-"$project_dir/build/DemoRTOSProject.elf"}
if [ "$#" -gt 0 ]; then shift; fi
qemu=${QEMU_SYSTEM_ARM:-qemu-system-arm}
if ! command -v "$qemu" >/dev/null 2>&1; then
    echo "QEMU not found. Set QEMU_SYSTEM_ARM to your custom qemu-system-arm; see doc/qemu.md." >&2
    exit 1
fi
if [ ! -f "$firmware" ]; then
    echo "Firmware not found: $firmware. Build it as described in doc/qemu.md." >&2
    exit 1
fi
exec "$qemu" -M blackpill-f411ce -kernel "$firmware" \
    -display none -monitor none -serial stdio "$@"
