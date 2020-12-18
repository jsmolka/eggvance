#pragma once

#include <shell/fmt.h>

#include "sdl2.h"

template<typename... Args>
void panic(const std::string& format, Args&&... args)
{
    const std::string message = fmt::format(format, std::forward<Args>(args)...);

    fmt::print(message);
    SDL_ShowSimpleMessageBox(0, "Panic", message.c_str(), NULL);

    std::exit(1);
}
