#pragma once

#include <shell/format.h>

template<typename... Args>
void panic(const std::string& format, Args&&... args)
{
    const auto message = shell::format(format, std::forward<Args>(args)...);

    shell::print(message);

    std::exit(1);
}
