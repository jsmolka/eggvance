#pragma once

#include "common/macros.h"
#include "common/utility.h"

struct BlendAlpha
{
    template<unsigned index>
    inline void write(u8 byte);

    int eva;  // EVA coefficient for layer A (0..16)
    int evb;  // EVA coefficient for layer B (0..16)
};

template<unsigned index>
inline void BlendAlpha::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0: 
        eva = std::min(bits<0, 5>(byte), 16); 
        break;

    case 1: 
        evb = std::min(bits<0, 5>(byte), 16); 
        break;

    default:
        UNREACHABLE;
        break;
    }
}
