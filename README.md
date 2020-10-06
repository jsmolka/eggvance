# eggvance
A Game Boy Advance emulator.

## State
Most aspects of the GBA are emulated with reasonably high accuracy. This results in the majority of games being playable with some minor issues here and there. The most notable missing features are audio, prefetch and RTC emulation.

## Usage
ROM and save files can be passed as command line arguments or dropped in the emulator window. Even though the included [BIOS](https://github.com/Nebuleon/ReGBA/tree/master/bios) should be enough to run most games, it is recommended to download the original one. Its location and many other things are customizable in the [config](eggvance/eggvance.ini).

```
eggvance [--help] [--config] [--save] [rom]
```

## Binaries
Binaries for Windows, Linux and macOS are built on each commit and can be found in the [Actions](https://github.com/jsmolka/eggvance/actions) tab on GitHub. Be aware that SDL2 needs to be installed on the latter two. An occasionally updated emscripten build is available on my [website](https://smolka.dev/eggvance/wasm).

## Building
Detailed build instructions can be found [here](BUILDING.md).

![mew](media/mew.png)
