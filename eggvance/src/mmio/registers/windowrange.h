#pragma once

#include "common/integer.h"
#include "common/macros.h"
#include "ppu/constants.h"

template<unsigned limit>
class WindowRange
{
public:
    inline void reset();
    inline bool contains(int x) const;

    template<unsigned index>
    inline void write(u8 byte);

    int min_hw;  // Min coordiante of window axis
    int max_hw;  // Max coordinate of window axis plus 1 (max > limit or min > max -> limit)

    int min;  // Min coordinate adjusted
    int max;  // Max coordinate adjusted

private:
    void adjust();
};

template<unsigned limit>
inline void WindowRange<limit>::reset()
{
    min_hw = 0;
    max_hw = 0;
    min    = 0;
    max    = 0;
}

template<unsigned limit>
inline bool WindowRange<limit>::contains(int x) const
{
    return x >= min && x < max;
}

template<unsigned limit>
template<unsigned index>
inline void WindowRange<limit>::write(u8 byte)
{
    static_assert(index <= 1);

    switch (index)
    {
    case 0: 
        max_hw = byte; 
        break;

    case 1: 
        min_hw = byte; 
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    adjust();
}

template<unsigned limit>
inline void WindowRange<limit>::adjust()
{
    min = min_hw;
    if (max_hw > limit || max_hw < min)
        max = limit;
    else
        max = max_hw;
}