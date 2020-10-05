#pragma once

#include <shell/env.h>

#if SHELL_RELEASE
#  include <shell/log/fatal.h>
#else
#  include <shell/log/all.h>
#endif

#include "base/sdl2.h"

class FatalExitSink : public shell::SinkInterface
{
public:
    void sink(const std::string&, const std::string& message, shell::Level level) override
    {
        if (level == shell::Level::Fatal)
        {
            SDL_ShowSimpleMessageBox(0, "Error", message.c_str(), NULL);
            std::exit(1);
        }
    }
};
