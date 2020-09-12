#pragma once

#include <eggcpt/fmt.h>

#include "base/sdl2.h"

template<typename... Args>
void exitWithMessage(const std::string& format, Args&&... args)
{
    std::string message = fmt::format(format, std::forward<Args>(args)...);

    SDL_ShowSimpleMessageBox(0, "Error", message.c_str(), NULL);
    fmt::print("{}\n", message);
    std::exit(1);
}
