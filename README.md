# eggvance
A Game Boy Advance emulator.

## Usage
ROM and save files can be passed as command line arguments or dropped in the emulator window. Even though the included [BIOS](https://github.com/Nebuleon/ReGBA/tree/master/bios) should be enough to run most games, it is recommended to download the original one. Its location and other things can be customized in the [config](eggvance/eggvance.ini).

```
Usage: eggvance [--help] [--config] [--save] [rom]

Keyword arguments:
--help, -h      Show this help text (optional)
--config, -c    Path to the config file (default: eggvance.ini)
--save, -s      Path to the save file (optional)

Positional arguments:
rom    Path to the ROM file (optional)
```

## Binaries
Prebuild binaries for Windows, Linux and macOS can be downloaded from the [Actions](https://github.com/jsmolka/eggvance/actions) tab on GitHub. Be aware that SDL2 needs to be installed on the latter two. An infrequently updated emscripten build can be found on my [website](https://smolka.dev/eggvance/wasm).

## Building
Detailed build instructions can be found [here](BUILDING.md).

![mew.png](media/mew.png)
