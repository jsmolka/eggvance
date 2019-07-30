#pragma once

#include "common/integer.h"

struct BackgroundOffset
{
    union
    {
        u8  offset_bytes[2];  // Offset bytes
        u16 offset;           // Offset (0..511)
    };
};
