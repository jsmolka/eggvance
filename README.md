# eggvance
A Game Boy Advance emulator.

## Usage
ROM and save files can be opened with the user interface or passed as command line arguments. The emulator comes with a bundled replacement [BIOS](https://github.com/Nebuleon/ReGBA/tree/master/bios) by Normmatt. It works for some games but I recommend using the original one for better compatibility.

```
usage:
  eggvance [--fullscreen] [--save <file>] [<rom>]

keyword arguments:
  -f, --fullscreen    fullscreen (optional)
  -s, --save          save file (optional)

positional arguments:
  rom    ROM file (optional)
```

## Binaries
Binaries for Windows, Linux and macOS are available as [nightly](https://nightly.link/jsmolka/eggvance/workflows/build/master) and [release](https://github.com/jsmolka/eggvance/releases) builds.

## Build
Detailed build instructions can be found [here](BUILD.md).

## Credit
These projects were invaluable resources while creating the emulator:
- [GBATEK](https://problemkaputt.de/gbatek.htm) and [No$GBA](https://problemkaputt.de/gba.htm) by Martin Korth
- [NanoBoyAdvance](https://github.com/fleroviux/NanoBoyAdvance) by fleroviux
- [mGBA](https://github.com/mgba-emu/mgba) and [suite](https://github.com/mgba-emu/suite) by endrift
- [higan](https://github.com/higan-emu/higan) by Near
- [Tonc](https://www.coranac.com/tonc/text/toc.htm) by cearn

![screenshot](screenshot.png)
