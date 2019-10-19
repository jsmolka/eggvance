#pragma once

#include "common/integer.h"

struct BackgroundOffset
{
    void reset();

    void writeByte(int index, u8 byte);

    int offset;  // Offset (0..511)
};
