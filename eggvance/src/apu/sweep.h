#pragma once

#include "base/bit.h"
#include "base/int.h"

class Sweep
{
public:
    void tick();
    void write(u8 byte);

    uint shift     = 0;
    uint direction = 0;
    uint time      = 0;
};
