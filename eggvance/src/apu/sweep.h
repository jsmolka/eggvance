#pragma once

#include "base/int.h"

class Sweep
{
public:
    void init();
    void init(uint freq);
    bool tick();
    void write(u8 byte);
    uint next() const;

    uint timer   = 0;
    uint enabled = 0;
    uint shift   = 0;
    uint negate  = 0;
    uint period  = 0;
    uint shadow  = 0;
};
