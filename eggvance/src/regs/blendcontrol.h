#pragma once

#include "register.h"

class BlendControl : public Register<2>
{
public:
    enum Mode
    {
        DISABLED = 0b00,
        ALPHA    = 0b01,
        WHITE    = 0b10,
        BLACK    = 0b11
    };

    void reset();

    u8 read(int index);
    void write(int index, u8 byte);

    int mode;
    int upper;
    int lower;
};
