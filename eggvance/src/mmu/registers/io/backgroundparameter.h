#pragma once

#include "common/integer.h"

struct BackgroundParameter
{
    union
    {
        u8 bytes[2];
        s16 value;
    };
};
