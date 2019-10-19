#pragma once

#include "common/integer.h"

class BlendBrightness
{
public:
    void reset();

    void write(u8 byte);

    int evy;  // Adjusted EVY value

private:
    struct
    {
        int evy;  // EVY coefficient for brightness (0..16)
    } regs;
};
