#pragma once

#ifdef _MSC_VER
#define COMPILER_MSVC 1
#else
#define COMPILER_MSVC 0
#endif

#ifdef __GNUC__
#define COMPILER_GCC 1
#else
#define COMPILER_GCC 0
#endif

#ifdef __clang__
#define COMPILER_CLANG 1
#else
#define COMPILER_CLANG 0
#endif

#ifdef __EMSCRIPTEN__
#define COMPILER_EMSCRIPTEN 1
#else
#define COMPILER_EMSCRIPTEN 0
#endif

#ifdef __MINGW32__
#define COMPILER_MINGW 1
#else
#define COMPILER_MINGW 0
#endif

#ifdef _WIN32
#define OS_WINDOWS 1
#else
#define OS_WINDOWS 0
#endif

#ifdef __linux__
#define OS_LINUX 1
#else
#define OS_LINUX 0
#endif

#ifdef __APPLE__
#define OS_APPLE 1
#else
#define OS_APPLE 0
#endif
