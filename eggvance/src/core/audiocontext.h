#pragma once

#include "base/eggcpt.h"
#include "base/sdl2.h"

class AudioContext
{
public:
    friend class Context;

private:
    void init();
    void deinit();
};
