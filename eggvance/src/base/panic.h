#pragma once

#include <shell/format.h>

#include "sdl2.h"

template<typename... Args>
void panic(const std::string& format, Args&&... args)
{
    const auto message = shell::format(format, std::forward<Args>(args)...);

    shell::print(message);
    SDL_ShowSimpleMessageBox(0, "Panic", message.c_str(), NULL);

    std::exit(1);
}
