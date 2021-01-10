#pragma once

#include "base/bit.h"
#include "base/int.h"

class Envelope
{
public:
    void tick();
    void write(u8 byte);

    uint volume    = 0;
    uint step_time = 0;
    uint direction = 0;
    uint initial   = 0;
};
