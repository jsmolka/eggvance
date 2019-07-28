#pragma once

#include "common/integer.h"

struct BackgroundReference
{
    union
    {
        u8 bytes[4];
        s32 value;
    };
    int internal;
};
