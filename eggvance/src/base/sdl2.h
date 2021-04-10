#pragma once

#include <string>
#include <shell/predef.h>
#include <shell/format.h>

#if SHELL_CC_MSVC
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_syswm.h>
#else
#  include "SDL.h"
#endif

template<typename... Args>
void showMessageBox(const std::string& title, const std::string& format, Args&&... args)
{
    std::string message = shell::format(format, std::forward<Args>(args)...);

    SDL_ShowSimpleMessageBox(0, title.c_str(), message.c_str(), NULL);
}
