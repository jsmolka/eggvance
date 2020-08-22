# eggvance
A Game Boy Advance emulator.

## Usage
```
eggvance rom.gba [save.sav]
```

Game and save files can also be dropped in the emulator window. Even though the included [BIOS](https://github.com/Nebuleon/ReGBA/tree/master/bios) should be enough to run most games, it is recommended to download the original one. Its location and other things can be customized in the [config](https://github.com/jsmolka/eggvance/blob/master/eggvance/eggvance.toml).

## Binaries
Binaries are available in two forms.

- [Nightly builds](https://github.com/jsmolka/eggvance/actions)
- [Release builds](https://github.com/jsmolka/eggvance/releases) (with profile guided optimization on Windows)

You need to install SDL2 on Linux and macOS.

## Building
Building requires a C++17 compiler.

Clone the submodules.

```
git submodule update --init --recursive
```

### Windows
Install SDL2 with [vcpkg](https://github.com/microsoft/vcpkg).

```
vcpkg install sdl2
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
Install and setup [emscripten](https://emscripten.org/docs/getting_started/downloads.html).

```
git clone https://github.com/emscripten-core/emsdk.git
./emsdk/emsdk install latest
./emsdk/emsdk activate latest
source ./emsdk/emsdk_env.sh
```

Build with CMake and `emcmake`.

```
mkdir eggvance/build
cd eggvance/build/
emcmake cmake ..
emmake make
```

A demo can be found [here](https://smolka.dev/eggvance/wasm).

## Credits
The following projects were invaluable resources while creating the emulator.
- [GBATEK](https://problemkaputt.de/gbatek.htm) and [No$GBA](https://problemkaputt.de/gba.htm) by Martin Korth
- [NanoboyAdvance](https://github.com/fleroviux/NanoboyAdvance) by fleroviux
- [mGBA](https://github.com/mgba-emu/mgba) by endrift
- [GBE+](https://github.com/shonumi/gbe-plus) by shonumi
- [Tonc](https://www.coranac.com/tonc/text/toc.htm) by cearn
