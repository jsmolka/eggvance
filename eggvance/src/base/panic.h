#pragma once

#include <shell/fmt/format.h>

#include "sdl2.h"

template<typename... Args>
void alert(const std::string& format, Args&&... args)
{
    const std::string message = fmt::format(format, std::forward<Args>(args)...);

    fmt::print(message);
    SDL_ShowSimpleMessageBox(0, "Message", message.c_str(), NULL);
}

template<typename... Args>
void panic(const std::string& format, Args&&... args)
{
    alert(format, std::forward<Args>(args)...);

    std::exit(0);
}
