#pragma once

#include "common/integer.h"

struct BackgroundOffset
{
    void reset();

    u8 readByte(int index);
    void writeByte(int index, u8 byte);

    int offset;  // Offset (0..511)
};
