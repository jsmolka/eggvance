# eggvance
An experimental Game Boy Advance emulator.

## Build
The emulator only supports Windows at the moment. It depends on the following libraries:
- [SDL2](https://www.libsdl.org/index.php)
- [fmt](https://github.com/fmtlib/fmt)

The header-only libraries are included as submodules and don't need to be installed manually. I recommend using [vcpkg](https://github.com/microsoft/vcpkg) to build and setup SDL2 with the following commands:
```batch
vcpkg install sdl2:x64-windows
vcpkg integrate install
```
