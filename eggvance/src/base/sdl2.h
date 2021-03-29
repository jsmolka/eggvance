#pragma once

#include <string>
#include <shell/format.h>
#include <shell/predef.h>

#if SHELL_CC_MSVC || SHELL_CC_EMSCRIPTEN
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_syswm.h>
#else
#  include "SDL.h"
#endif

inline void showMessageBox(const std::string& title, const std::string& message)
{
    shell::print("{}\n", message);

    SDL_ShowSimpleMessageBox(0, title.c_str(), message.c_str(), NULL);
}
