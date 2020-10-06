#pragma once

#include <shell/fmt.h>

#include "base/sdl2.h"

template<typename String, typename... Args>
void exit(const String& format, Args&&... args)
{
    auto message = fmt::format(format, std::forward<Args>(args)...);

    fmt::print(message);
    SDL_ShowSimpleMessageBox(0, "Exit", message.c_str(), NULL);

    std::exit(0);
}
