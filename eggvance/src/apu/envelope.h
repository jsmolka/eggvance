#pragma once

#include "base/int.h"

class Envelope
{
public:
    void init();
    void tick();
    bool isEnabled() const;

    u8 read() const;
    void write(u8 byte);

    uint period   = 0;
    uint increase = 0;
    uint initial  = 0;
    uint timer    = 0;
    uint volume   = 0;
};
