#pragma once

#include "common/integer.h"

struct BackgroundParameter
{
    union
    {
        u8  param_bytes[2];  // Affine parameter bytes
        s16 param;           // Affine parameter
    };
};
