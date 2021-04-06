#pragma once

#include <shell/array.h>

#include "point.h"
#include "base/int.h"

class Matrix : public shell::array<s16, 4>
{
public:
    constexpr Point operator*(const Point& point) const
    {
        return Point(
            (*this)[0] * point.x + (*this)[1] * point.y,
            (*this)[2] * point.x + (*this)[3] * point.y);
    }
};
