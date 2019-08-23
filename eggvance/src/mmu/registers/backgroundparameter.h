#pragma once

#include "common/utility.h"

struct BackgroundParameter
{
    template<unsigned index>
    inline void write(u8 byte);

    union
    {
        u8  param_b[2];  // Affine parameter bytes
        s16 param;       // Affine parameter
    };
};

template<unsigned index>
inline void BackgroundParameter::write(u8 byte)
{
    static_assert(index <= 1);

    bytes(&param)[index] = byte;
}
