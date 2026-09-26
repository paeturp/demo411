
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Discover the cross compiler from PATH. This works with Homebrew on macOS and
# with a conventional Arm GNU Toolchain installation on Linux.
find_program(ARM_NONE_EABI_GCC arm-none-eabi-gcc REQUIRED)
find_program(ARM_NONE_EABI_GXX arm-none-eabi-g++ REQUIRED)
set(CMAKE_C_COMPILER   "${ARM_NONE_EABI_GCC}")
set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_GXX}")
set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_GCC}")


# Find the rest of the embedded programs
find_program(GDB      arm-none-eabi-gdb)
find_program(OPENOCD  openocd)
find_program(OBJCOPY  arm-none-eabi-objcopy REQUIRED)
find_program(OBJDUMP  arm-none-eabi-objdump REQUIRED)
find_program(SIZE     arm-none-eabi-size REQUIRED)
find_program(OBJNAMES arm-none-eabi-nm)
find_program(ARCHIVE  arm-none-eabi-ar)
find_program(STRIP    arm-none-eabi-strip)
find_program(STRINGS  arm-none-eabi-strings)
find_program(GCOV     arm-none-eabi-gcov)
