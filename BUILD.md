# Build
A C++17 compiler is a base requirement.

## Clone
Use [Git](https://git-scm.com/) to clone the repository and its submodules.

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

Install [SDL2](https://libsdl.org/download-2.0.php) and [OpenGL](https://www.opengl.org/).

```
> vcpkg install sdl2:x64-windows
> vcpkg install opengl:x64-windows
```

### Linux
Install [SDL2](https://libsdl.org/download-2.0.php) and [GTK](https://www.gtk.org/).

```
$ [sudo] apt-get install libsdl2-dev
$ [sudo] apt-get install libgtk-3-dev
```

### macOS
Install [Homebrew](https://brew.sh/).

```
$ /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Install [SDL2](https://libsdl.org/download-2.0.php).

```
$ brew install sdl2
```

## Build

### Windows
Build the Visual Studio solution. This can also be done on the command line.

```
> call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
> msbuild /property:Configuration=Release eggvance.sln
```

### Linux and macOS
Build with [CMake](https://cmake.org/).

```
$ mkdir eggvance/build
$ cd eggvance/build
$ cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-march=native" ..
$ make -j 4
```
