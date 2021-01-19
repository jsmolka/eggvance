#pragma once

#include "base/int.h"

class Sweep
{
public:
    void init();
    void init(uint freq);
    void write(u8 byte);
    uint calculate() const;

    uint shift  = 0;
    uint negate = 0;
    uint period = 0;
    uint shadow = 0;
    uint timer  = 0;
    uint enable = 0;
};
