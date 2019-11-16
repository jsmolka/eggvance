#pragma once

#include "dimensions.h"

struct Point
{
    constexpr Point(int x, int y)
        : x(x), y(y) { }

    constexpr bool inBounds(const Dimensions& dims) const
    {
        return x >= 0
            && y >= 0
            && x < dims.w
            && y < dims.h;
    }

    int x;
    int y;
};
