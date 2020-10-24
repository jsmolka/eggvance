#pragma once

#include "point.h"
#include "base/int.h"

class Matrix
{
public:
    constexpr Matrix(s16 pa, s16 pb, s16 pc, s16 pd)
        : pa(pa)
        , pb(pb)
        , pc(pc)
        , pd(pd) {}

    constexpr Point operator*(const Point& point) const
    {
        return Point(
            pa * point.x + pb * point.y,
            pc * point.x + pd * point.y);
    }

    s16 pa;
    s16 pb;
    s16 pc;
    s16 pd;
};
