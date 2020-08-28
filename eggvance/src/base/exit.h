#pragma once

#include <string>
#include <utility>

#include <eggcpt/fmt.h>

#include "base/sdl2.h"

inline void exitWithMessage(const std::string& message)
{
    SDL_ShowSimpleMessageBox(0, "Error", message.c_str(), NULL);
    fmt::print("{}\n", message);
    std::exit(1);
}
