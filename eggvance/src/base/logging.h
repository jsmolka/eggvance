#pragma once

#include <eggcpt/env.h>

#if EGGCPT_RELEASE
#  include <eggcpt/log/fatal.h>
#else
#  include <eggcpt/log/all.h>
#endif

#include "base/sdl2.h"

class FatalExitSink : public eggcpt::SinkInterface
{
public:
    void sink(const std::string&, const std::string& message, eggcpt::Level level) override
    {
        if (level == eggcpt::Level::Fatal)
        {
            SDL_ShowSimpleMessageBox(0, "Error", message.c_str(), NULL);
            std::exit(1);
        }
    }
};
