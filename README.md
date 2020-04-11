<div align="center">
  <img alt="icon" height="128" src="https://raw.githubusercontent.com/jsmolka/icon/master/bin/icon-512.png">
</div>

# eggvance
A Game Boy Advance emulator.

## Usage
```
eggvance rom.gba [backup.sav]
```

Games and save files can also be dropped in the emulator window. Even though the included [BIOS](https://github.com/Nebuleon/ReGBA/tree/master/bios) should be enough to run most games, it is recommended to download the original one. Its location and other things can be customized in the [config](https://github.com/jsmolka/eggvance/blob/master/eggvance/eggvance.toml).

## Binaries
Binaries are available in two forms.

- [Nightly builds](https://github.com/jsmolka/eggvance/actions)
- [Release builds](https://github.com/jsmolka/eggvance/releases) (with profile guided optimization on Windows)

You need to install SDL2 on Linux and macOS.

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

### macOS
Install SDL2 with [homebrew](https://brew.sh/).

```
brew install sdl2
```

Build like on Linux.

### Emscripten
Setup [emscripten](https://emscripten.org/docs/getting_started/downloads.html).

```
mkdir eggvance/build
cd eggvance/build/
emmake cmake -DPLATFORM=EMSCRIPTEN ..
emmake make
```

## Issues
- no audio emulation
- no link cable emulation
- no real-time clock emulation


## Credits
The following projects were invaluable resources while creating the emulator.
- [GBATEK](https://problemkaputt.de/gbatek.htm) and [No$GBA](https://problemkaputt.de/gba.htm) by Martin Korth
- [NanoboyAdvance](https://github.com/fleroviux/NanoboyAdvance) by fleroviux
- [GBE+](https://github.com/shonumi/gbe-plus) by shonumi
- [Tonc](https://www.coranac.com/tonc/text/toc.htm) by cearn
