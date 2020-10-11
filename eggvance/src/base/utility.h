#pragma once

#include <shell/fmt.h>

#include "base/sdl2.h"

template<typename... Args>
void message(const std::string& format, Args&&... args)
{
    const std::string message = fmt::format(format, std::forward<Args>(args)...);

    fmt::print(message);
    SDL_ShowSimpleMessageBox(0, "Attention", message.c_str(), NULL);
}

template<typename... Args>
void exit(const std::string& format, Args&&... args)
{
    message(format, std::forward<Args>(args)...);

    std::exit(0);
}
