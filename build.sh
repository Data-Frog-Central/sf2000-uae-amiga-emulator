#!/bin/bash
# SF2K-UAE SF2000 v058 - Self-contained build script
# v058: Menu redesign - About, Settings with Kickstart/RAM, per-game config prep
# v057: Fix Disk Shuffler - detect 5+ disks, reset disabled mask on shuffle
# v056: Disk Shuffler moved to first menu position for easier access
# v055: Disk Shuffler - rotate disks, disable unused drives to save chip RAM
# v054: L+R 3sec hold for mouse, FrogJoy2 mouse fix, better menu labels
# v053: Fix mouse - add L+R toggle for mouse mode (like v007), remove broken save states from menu
# Builds UAE4ALL core and links to core_87000000 binary
# All files in one directory - ready for GitHub

set -e

TOOLCHAIN=/tmp/mips32-mti-elf/2019.09-03-2/bin
GCC=$TOOLCHAIN/mips-mti-elf-g++
OBJCOPY=$TOOLCHAIN/mips-mti-elf-objcopy

echo "=== SF2K-UAE SF2000 Build Script (v058) ==="
echo "Building UAE4ALL libretro core..."

# Step 1: Build UAE4ALL core
make -f Makefile.libretro clean platform=sf2000
make -f Makefile.libretro platform=sf2000 -j4

# Check if .a was created
if [ ! -f uae4all_libretro_sf2000.a ]; then
    echo "ERROR: uae4all_libretro_sf2000.a not created!"
    exit 1
fi

echo "UAE4ALL core built: $(ls -lh uae4all_libretro_sf2000.a)"

# Step 2: Link to SF2000 binary
echo "Linking to core_87000000..."

$GCC -Wl,-Map=core.elf.map \
    -EL -march=mips32 -mtune=mips32 -msoft-float \
    -Wl,--gc-sections --static -z max-page-size=32 \
    -e __core_entry__ \
    -Tlinker/core.ld linker/bisrv_08_03-core.ld \
    -o core.elf \
    -Wl,--start-group \
    linker/core_api.o linker/lib.o linker/debug.o linker/video_sf2000.o \
    uae4all_libretro_sf2000.a linker/libretro-common.a \
    -lc \
    -Wl,--end-group

$OBJCOPY -O binary \
    -R .MIPS.abiflags -R .note.gnu.build-id -R ".rel*" \
    core.elf core_87000000

# Check result
if [ -f core_87000000 ]; then
    SIZE=$(ls -lh core_87000000 | awk '{print $5}')
    echo "=== BUILD SUCCESS ==="
    echo "Output: core_87000000 ($SIZE)"
    echo "Copy this file to your SF2000 SD card cores/ folder"
else
    echo "ERROR: core_87000000 not created!"
    exit 1
fi
