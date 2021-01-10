#pragma once

#include "base/int.h"

class Envelope
{
public:
    void init();
    void tick();
    void write(u8 byte);
    bool enable() const;

    uint period      = 0;
    uint period_init = 0;
    uint volume      = 0;
    uint volume_init = 0;
    uint direction   = 0;
};
