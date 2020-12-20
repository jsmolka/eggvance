#pragma once

#include "point.h"
#include "base/int.h"

class Matrix
{
public:
    constexpr Matrix()
        : pa(0)
        , pb(0)
        , pc(0)
        , pd(0) {}

    constexpr Matrix(s16 pa, s16 pb, s16 pc, s16 pd)
        : pa(pa)
        , pb(pb)
        , pc(pc)
        , pd(pd) {}

    constexpr s16& operator[](uint index)
    {
        return data[index];
    }

    constexpr s16 operator[](uint index) const
    {
        return data[index];
    }

    constexpr Point operator*(const Point& point) const
    {
        return Point(
            pa * point.x + pb * point.y,
            pc * point.x + pd * point.y);
    }

    union
    {
        struct
        {
            s16 pa;
            s16 pb;
            s16 pc;
            s16 pd;
        };
        s16 data[4];
    };
};
