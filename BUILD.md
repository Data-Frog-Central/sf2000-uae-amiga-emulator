# Building UAE4ALL for SF2000/GB300

## Requirements

- Windows with WSL (Ubuntu)
- MIPS toolchain: `/tmp/mips32-mti-elf/2019.09-03-2/` or `/opt/mips32-mti-elf/2019.09-03-2/`
- Multicore framework (see below)

## Build Scripts

| Script | Platform | Framework |
|--------|----------|-----------|
| `build_sf2000_mc.bat` | SF2000 | madcock multicore |
| `build_gb300_mc.bat` | GB300 | madcock multicore |
| `build_sf2000_frogui.bat` | SF2000 | FrogUI (Trademarked69) |
| `build_gb300_frogui.bat` | GB300 V2 | FrogUI (Trademarked69) |

## Configuration

Edit `MULTICORE_DIR` at the top of each .bat file:

```batch
REM === CONFIGURATION - EDIT THESE PATHS ===
set MULTICORE_DIR=C:\Path\To\Your\multicore
REM =========================================
```

### Framework Repositories

- **madcock multicore**: `sf2000_multicore_official` / `gb300_multicore`
- **FrogUI**: `sf2000_multicore` by Trademarked69

## Usage

Simply run the appropriate .bat file:

```
build_sf2000_mc.bat
```

Output: `core_87000000` in the project directory.

## Deployment

Copy `core_87000000` to `SD:\cores\amiga\` on your device.
