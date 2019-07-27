#pragma once

#include "common/integer.h"

struct BackgroundOffset
{
    union
    {
        u8 bytes[2];
        u16 offset;   // 0-8: offset (0..511)
    };
};
