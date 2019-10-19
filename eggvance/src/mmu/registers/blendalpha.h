#pragma once

#include "common/integer.h"

class BlendAlpha
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);
    
    int eva;  // Adjusted EVA value
    int evb;  // Adjusted EVB value

private:
    struct
    {
        int eva;  // EVA coefficient for layer A (0..16)
        int evb;  // EVA coefficient for layer B (0..16)
    } regs;
};
