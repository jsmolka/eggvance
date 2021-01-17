#pragma once

#include "base/bit.h"
#include "base/int.h"

class Sweep
{
public:
    void init();
    void write(u8 byte);
    bool isEnabled() const;

    uint shift      = 0;
    uint negate     = 0;
    uint timer_init = 0;
    uint timer      = 0;
    uint enabled    = 0;
    uint shadow     = 0;
};
