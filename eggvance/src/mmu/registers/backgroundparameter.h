#pragma once

#include "common/integer.h"

class BackgroundParameter
{
public:
    void write(int index, u8 byte);

    union
    {
        u8  param_b[2];  // Affine parameter bytes
        s16 param;       // Affine parameter
    };
};
