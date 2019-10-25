#pragma once

#include "register.h"

enum BlendMode
{
    BLD_DISABLED = 0b00,
    BLD_ALPHA    = 0b01,
    BLD_WHITE    = 0b10,
    BLD_BLACK    = 0b11
};


class BlendControl : public Register<2>
{
public:
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int mode;
    int upper;
    int lower;
};
