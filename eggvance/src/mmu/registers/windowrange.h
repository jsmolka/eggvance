#pragma once

#include "common/integer.h"
#include "common/macros.h"

template<int limit>
class WindowRange
{
public:
    void reset();

    bool contains(int x) const;

    void writeByte(int index, u8 byte);

    int min;  // Min coordinate adjusted
    int max;  // Max coordinate adjusted

private:
    struct
    {
        int min;  // Min coordiante of window axis
        int max;  // Max coordinate of window axis plus 1 (max > limit or min > max -> limit)
    } reg;

    void adjust();
};

template<int limit>
void WindowRange<limit>::reset()
{
    reg.min = 0;
    reg.max = 0;
    min     = 0;
    max     = 0;
}

template<int limit>
bool WindowRange<limit>::contains(int x) const
{
    return x >= min && x < max;
}

template<int limit>
void WindowRange<limit>::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    switch (index)
    {
    case 0: reg.max = byte; break;
    case 1: reg.min = byte; break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    adjust();
}

template<int limit>
void WindowRange<limit>::adjust()
{
    min = reg.min;
    max = reg.max;

    if (max > limit || max < min)
        max = limit;
}