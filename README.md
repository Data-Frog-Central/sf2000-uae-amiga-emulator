# SF2K-UAE - Amiga 500 Emulator for SF2000

**Version:** v0.58
**Author:** Grzegorz Korycki (@the_q_dev on Telegram)
**Based on:** UAE4ALL

Amiga 500 (OCS/ECS) emulator for the SF2000 handheld gaming device running Multicore firmware.

## Requirements

This emulator requires **Multicore firmware** installed on your SF2000.

### Installing Multicore (quick guide)

1. Download Multicore from [GitHub releases](https://github.com/madcock/sf2000_multicore/releases)
2. Extract to a fresh SD card (FAT32 formatted)
3. Copy your existing `bios/` folder to the new SD card
4. Insert SD card and power on SF2000

## Installation

1. Copy `core_87000000` to: `cores/amiga/` on your SD card
2. Place your ADF game files in: `ROMS/` folder
3. Place Kickstart ROM files in: `bios/` folder

### Required Kickstart ROMs

Place at least one of these in the `bios/` folder:

| Filename | Version |
|----------|---------|
| `kick13.rom` | Kickstart 1.3 |
| `kick20.rom` | Kickstart 2.0 |
| `kick30.rom` | Kickstart 3.0 |

The emulator will automatically select the best available ROM.

## Multi-Disk Games

The emulator supports multi-disk games (up to 4 simultaneous drives).

**How it works:**
- Place all disk images for a game with the same filename prefix
- Example: `Monkey Island (Disk 1).adf`, `Monkey Island (Disk 2).adf`, etc.
- The emulator automatically loads disks into DF0-DF3 in alphabetical order
- Use **Disk Shuffler** menu (START -> Disks) to rotate disks when game asks to swap

## Controls

| Button | Action |
|--------|--------|
| D-Pad | Joystick movement |
| A / B | Fire button |
| X | Jump (Up direction) |
| START | Open menu |
| SELECT | Toggle virtual keyboard |
| Y | Quick disk swap |
| L+R (hold 3 sec) | Toggle mouse mode |

### Menu Options

- **Disks** - Disk Shuffler for multi-disk games
- **FrogJoy1/2** - Controller assignment (Joy Port1, Joy Port0, Mouse)
- **Frameskip** - 0-5 (higher = faster but choppier)
- **Sound** - OFF / ON / EMUL
- **CPU** - Timing multiplier (2 = normal)
- **Y-Offset** - Vertical screen adjustment
- **Floppy** - Disk speed (1x-MAX turbo)
- **Settings** - Kickstart/RAM selection, Save Config
- **About** - Version and credits

## Known Bugs

- **Save states** - Loading save states does not work
- **Per-game config** - Custom configuration per game not implemented
- **Disk Shuffler** - May not work correctly with some multi-disk games

## Building from Source

Requires MIPS MTI cross-compiler toolchain.

```bash
# In WSL/Linux
./build.sh
```

Output: `core_87000000`

## Credits

- UAE4ALL team
- Chips (original SF2000 port foundation)
- Madcock (SF2000 Multicore framework)

## License

GPL v2 (inherited from UAE)
