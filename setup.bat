@echo off

rem Clone submodules
git submodule init
git submodule update

rem Find or build vcpkg
where vcpkg >nul 2>nul
if errorlevel 1 (
    pushd "deps/vcpkg"
    if not exist "vcpkg.exe" (
        call bootstrap-vcpkg.bat
    )
)

rem Install dependencies
vcpkg install fmt:x64-windows
vcpkg install sdl2:x64-windows

rem Make packages globally available
vcpkg integrate install

popd
