#pragma once

#include "common/integer.h"

class BackgroundReference
{
public:
    void write(int index, u8 byte);

    union
    {
        u8  ref_b[4];  // Reference point bytes
        s32 ref;       // Reference point
    };
    int internal;  // Internal register (ref copied during V-Blank or write)
};
