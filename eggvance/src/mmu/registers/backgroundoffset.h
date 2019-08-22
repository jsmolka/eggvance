#pragma once

#include "common/integer.h"

class BackgroundOffset
{
public:
    void write(int index, u8 byte);

    union
    {
        u8  offset_b[2];  // Offset bytes
        u16 offset;       // Offset (0..511)
    };
};
