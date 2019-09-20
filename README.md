# eggvance
An experimental Game Boy Advance emulator.

## State
It's hard to give an accurate statement about the current state of the emulator because I can't possibly test all games myself. However, I can say that it boots and emulates most of the major titles to a certain extent (that usually being the tutorial). I'll do my best to maintain a [list](www.eggception.de/eggvance/compatibility/) of fully tested games and to write a [progress report](www.eggception.de/eggvance/progress/) every once in a while.

## Usage
Before using the emulator you need to download a GBA BIOS file and place it next to the application (call it `bios.bin`). Starting a game can be done by passing it as a command line argument or dropping it in the opened emulator window.

## Building
There are some Windows specific parts in the code. Porting those to other platforms shouldn't be to hard and will be done once the emulator reaches a more mature state. The current external dependencies are:
- [SDL2](https://www.libsdl.org/index.php)
- [fmt](https://github.com/fmtlib/fmt)

The header-only libraries are included as submodules and don't need to be installed manually. For building and setting up SDL2 I recommend using [vcpkg](https://github.com/microsoft/vcpkg). It can be done with the following commands:
```batch
vcpkg install sdl2:x64-windows
vcpkg integrate install
```

Now you should be able to build the emulator using the Visual Studio project.
