# eggvance
A Game Boy Advance emulator.

## State
Bad news first: there is no audio. Apart from that the emulator is able to boot every tested game and the vast majority of them run without any issues. I'll do my best to maintain a [list](https://eggception.de/eggvance/compatibility/) of fully tested games and to write a [progress report](https://eggception.de/eggvance/progress/) every once in a while.

## Usage
The emulator requires a BIOS file in order to work. Its location and many other things can be customized in the [eggvance.toml](https://github.com/jsmolka/eggvance/blob/master/eggvance/eggvance.toml) configuration file. A game can be started by either passing it as a command line argument or dropping it in the opened emulator window (please note that only paths with ANSI characters are supported at the moment). A compiled version can be downloaded [here](https://github.com/jsmolka/eggvance/releases).

## Building
There are some Windows specific parts in the code (like MSVC intrinsic functions and WinAPI calls). Porting those to other platforms shouldn't be to hard and will be done at some point in the future. The current external dependencies are:
- [fmt](https://github.com/fmtlib/fmt)
- [SDL2](https://www.libsdl.org/index.php)
- [tinytoml](https://github.com/mayah/tinytoml)

The header-only libraries are included as submodules and don't need to be installed manually. For building and setting up SDL2 I recommend using [vcpkg](https://github.com/microsoft/vcpkg). It can be done with the following commands:
```
vcpkg install sdl2:x64-windows
vcpkg integrate install
```

Now you should be able to build the Visual Studio project.

## Credits
The following projects were invaluable resources while creating the emulator:
- [GBATEK](https://problemkaputt.de/gbatek.htm) and [No$GBA](https://problemkaputt.de/gba.htm) by Martin Korth
- [NanoboyAdvance](https://github.com/fleroviux/NanoboyAdvance) by fleroviux
- [GBE+](https://github.com/shonumi/gbe-plus) by shonumi
- [Tonc](https://www.coranac.com/tonc/text/toc.htm) by Jasper Vijn
