# Building
A C++17 compliant compiler is required on all platforms.

## Clone
Use [git](https://git-scm.com/) to clone the repository and its submodules.

```
> git clone https://github.com/jsmolka/eggvance
> cd eggvance
> git submodule update --init
```

## Dependencies

### Windows
Install and setup [vcpkg](https://github.com/microsoft/vcpkg).

```
> git clone https://github.com/microsoft/vcpkg
> cd vcpkg
> bootstrap-vcpkg.bat
> vcpkg integrate install
```

Install [SDL2](https://libsdl.org/download-2.0.php).

```
> vcpkg install sdl2:x64-windows
```

### Linux
Install [SDL2](https://libsdl.org/download-2.0.php).

```
$ [sudo] apt-get install libsdl2-dev
```

### macOS
Install [homebrew](https://brew.sh/).

```
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
```

Install [SDL2](https://libsdl.org/download-2.0.php).

```
$ brew install sdl2
```

## Build

### Windows
Build the Visual Studio solution. This can also be done on the command line.

```
> call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
> msbuild /property:Configuration=Release eggvance.sln
```

### Linux / macOS
Build with [cmake](https://cmake.org/).

```
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native" ..
$ make -j 4
```
