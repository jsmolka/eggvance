#pragma once

#include "common/utility.h"

struct BackgroundParameter
{
    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    s16 parameter;  // Affine parameter
};

template<unsigned index>
inline void BackgroundParameter::write(u8 byte)
{
    static_assert(index <= 1);

    bytes(&parameter)[index] = byte;
}
