# Run and test demo411 in QEMU

Use a custom QEMU build containing the experimental `blackpill-f411ce` machine.
Standard QEMU releases do not provide this model. The emulator is an external
build dependency: all firmware building, application launch and integration
testing stay in this repository.

## Ubuntu prerequisites

Install the firmware build dependencies:

```sh
sudo apt update
sudo apt install git cmake make ninja-build python3 gcc-arm-none-eabi \
  libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
```

CMake 3.25 or newer is required. Ubuntu 24.04 or newer supplies a suitable
version; on older systems, install a newer CMake first. The embedded C++ library
is needed because the current build also creates C++ test executables.
See the [README](../README.md#build) for other platforms and toolchain options.

Build the experimental QEMU fork separately, following its
`docs/system/arm/blackpill.rst` instructions. Ubuntu needs no `--disable-pvg`
option. Use the fork's `build/qemu-system-arm`, not a distribution QEMU binary
without this board model. No system-wide QEMU installation is required.

The following workflow has been tested on macOS; Ubuntu verification remains
outstanding.

## Build the firmware

From this repository's root, configure a separate firmware build:

```sh
cmake -S . -B build/qemu -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$PWD/cmake/toolchain_arm.cmake"
cmake --build build/qemu
```

This uses the Arm GNU bare-metal toolchain described in the README and leaves
any existing `build/DemoRTOSProject.elf` alone.

## Run

Select the custom emulator and run:

```sh
export QEMU_SYSTEM_ARM=/absolute/path/to/qemu-experimental/build/qemu-system-arm
./scripts/run-demo411.sh
```

`QEMU_SYSTEM_ARM` is configurable; replace the placeholder with the absolute
path to your QEMU executable. Check that it contains the board before launching:

```sh
"$QEMU_SYSTEM_ARM" -machine help | grep blackpill-f411ce
```

If unset, the scripts look for `qemu-system-arm` on `PATH`. The launcher locates
its own project directory, so it also works when invoked from another directory.
Its default firmware is `build/qemu/DemoRTOSProject.elf` in this repository.

You should see the firmware version, `00:00:00`, and a `>` prompt. Enter commands
followed by Return:

- `?`: command menu
- `TS?`: read time
- `TS123456`: set time to 12:34:56
- `TD?`: read date
- `TD260926`: set date to 2026-09-26 (see the parser caveat below)

USART2 is connected to the terminal. Ctrl-C exits. The GPIO tasks run silently;
PA6 toggles every 1.5 seconds and PA7 every 0.8 seconds. These are not the board's
PC13 LED. The emulator has no graphical board display.

An optional first argument selects an ELF or BIN. Remaining arguments go to
QEMU. For example, to record GPIO transitions:

```sh
./scripts/run-demo411.sh build/qemu/DemoRTOSProject.elf \
  -trace enable=stm32f411_gpio -D build/qemu-gpio.log
```

To debug, instead append `-S -gdb tcp:127.0.0.1:1234`, load the same ELF in an
Arm-aware GDB, and use `target remote 127.0.0.1:1234`, `break main`, `continue`.

## Integration test

After building and setting `QEMU_SYSTEM_ARM` as above:

```sh
python3 scripts/test-demo411.py
```

All three defaults can be overridden:

```sh
python3 scripts/test-demo411.py \
  --qemu "$QEMU_SYSTEM_ARM" \
  --firmware build/qemu/DemoRTOSProject.elf \
  --output build/qemu-test
```

This is the firmware integration test for the custom system emulator, separate
from the project's existing CTest unit tests.

The test uses Python's standard library and starts/stops its own QEMU instance.
It exercises the firmware banner, command menu, time/date commands, RTC advance,
FreeRTOS GPIO timing, and reset with RTC preservation. Repeat with the BIN file
to check raw-image loading. QEMU must use its default log trace backend.

Results are printed as JSON. Success exits with status 0; a failed assertion
exits nonzero and prints a traceback. The output directory retains:

- `report.json`: checks completed and the firmware SHA-256 hash.
- `serial.log`: firmware output and command responses.
- `qemu.log`: diagnostics and GPIO transitions in virtual nanoseconds.

```sh
cat build/qemu-test/report.json
cat build/qemu-test/serial.log
less build/qemu-test/qemu.log
```

The integration test enables instruction-counted virtual time with
`-icount shift=7,sleep=off`. Guest time can progress faster than wall time; it is
not a cycle-accurate hardware timing measurement. Interactive runs omit this.

## Application and model limitations

The current firmware date parser in `lib/menu/src/tmenu.c` passes byte-sized
pointers to `sscanf` integer conversions. This is an existing C type mismatch;
a passing date-command test does not prove it harmless. This reorganization
does not change firmware behavior.

The custom model supports the subset used here: Cortex-M4F, flash/SRAM,
startup clocks, USART2, GPIOA and the RTC. RTC state survives a system reset but
not a new emulator process. Full GPIO mux/electrical behavior, USB, DMA, I2C,
SPI, flash programming and migration are unsupported. Consult the QEMU model's
`docs/system/arm/blackpill.rst` for its complete scope. Hardware comparison has
not been performed.
