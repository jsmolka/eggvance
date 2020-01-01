<div align="center">
  <img alt="icon" height="128" src="https://raw.githubusercontent.com/jsmolka/eggvance/master/eggvance/icon/icon.png">
</div>

# eggvance
A Game Boy Advance emulator.

## Usage
A game can be started by either passing it as a command line argument or dropping it in the opened window. Even though the included [BIOS](https://github.com/Nebuleon/ReGBA/tree/master/bios) should be enough to run most games, it is recommended to use the original. Its location and many other things can be customized in the [config](https://github.com/jsmolka/eggvance/blob/master/eggvance/eggvance.toml). Windows binaries for the latest release can be found [here](https://github.com/jsmolka/eggvance/releases).

## Building
A C++17 compiler is required to build the emulator.

Clone the submodules.

```
git submodule update --init
```

### Windows
Install SDL2 with [vcpkg](https://github.com/microsoft/vcpkg).

```
vcpkg install sdl2:x64-windows
vcpkg integrate install
```

Build the Visual Studio solution.

### Linux
Install SDL2 with the package manager.

```
sudo apt-get install libsdl2-dev
```

Build with CMake.

```
mkdir eggvance/build
cd eggvance/build/
cmake ..
make
```

## Issues
- no support for UTF-8 paths on Windows
- no audio emulation
- no RTC emulation

## Credits
The following projects were invaluable resources while creating the emulator.
- [GBATEK](https://problemkaputt.de/gbatek.htm) and [No$GBA](https://problemkaputt.de/gba.htm) by Martin Korth
- [NanoboyAdvance](https://github.com/fleroviux/NanoboyAdvance) by fleroviux
- [GBE+](https://github.com/shonumi/gbe-plus) by shonumi
- [Tonc](https://www.coranac.com/tonc/text/toc.htm) by cearn
