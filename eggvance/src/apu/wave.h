#pragma once

#include "channel.h"

class Wave : public Channel<0x00E0, 0xE000, 0x4000>
{
public:
    void tick();
};
