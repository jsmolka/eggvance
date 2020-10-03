# Building
This document contains detailed instructions on how to build eggvance from source. A basic requirement for this is a C++17 compliant compiler.

## Clone
Install [git](https://git-scm.com/), clone the repository and initialize the submodules.

```cmd
> git clone https://github.com/jsmolka/eggvance
> cd eggvance
> git submodule update --init --recursive
```

## Dependencies

### Windows
Install and setup [vcpkg](https://github.com/microsoft/vcpkg).

```cmd
> git clone https://github.com/microsoft/vcpkg
> cd vcpkg
> bootstrap-vcpkg.bat
> vcpkg integrate install
```

Install [SDL2](https://libsdl.org/download-2.0.php).

```cmd
> vcpkg install sdl2:x64-windows
```

### Linux
Install [SDL2](https://libsdl.org/download-2.0.php).

```sh
$ [sudo] apt-get install libsdl2-dev
```

### macOS
Install [homebrew](https://brew.sh/).

```sh
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```

Install [SDL2](https://libsdl.org/download-2.0.php).

```sh
$ brew install sdl2
```

### Emscripten
Install and setup [emscripten](https://emscripten.org/).

```sh
$ git clone https://github.com/emscripten-core/emsdk.git
$ cd emsdk
$ ./emsdk install latest
$ ./emsdk activate latest
$ source ./emsdk_env.sh
```

## Build

### Windows
Build the Visual Studio solution. This can also be done with the command line.

```cmd
> call "C:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Auxiliary/Build/vcvars64.bat"
> msbuild /property:Configuration=Release eggvance.sln
```

### Linux and macOS
Build with [cmake](https://cmake.org/).

```sh
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native" ..
$ make -j 4
```

### Emscripten
Build with `emcmake` and `emmake`.

```sh
$ mkdir build
$ cd build
$ emcmake cmake -DCMAKE_BUILD_TYPE=Release ..
$ emmake make -j 4
```
