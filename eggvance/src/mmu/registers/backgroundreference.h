#pragma once

#include "common/integer.h"

struct BackgroundReference
{
    union
    {
        u8  ref_b[4];  // Reference point bytes
        s32 ref;       // Reference point
    };
    int internal;  // Internal register (ref copied during V-Blank or write)
};
