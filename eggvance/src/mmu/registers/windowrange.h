#pragma once

#include "common/integer.h"
#include "common/macros.h"

struct WindowRange
{
    void reset();

    template<unsigned index>
    inline void write(u8 byte);

    int min;  // Min coordiante of window axis
    int max;  // Max coordinate of window axis plus 1 (max > limit or min > max -> limit) 
};

template<unsigned index>
inline void WindowRange::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0: 
        max = byte; 
        break;

    case 1: 
        min = byte; 
        break;

    default:
        UNREACHABLE;
        break;
    }
}
