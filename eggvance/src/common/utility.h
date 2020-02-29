#pragma once

#include "common/integer.h"

template<uint... values>
inline uint stacklut(std::size_t index)
{
    constexpr uint lut[sizeof...(values)] = { values... };

    return lut[index];
}
