#pragma once

#include "common/integer.h"
#include "point.h"

struct Matrix
{
    constexpr Matrix(int pa, int pb, int pc, int pd)
        : pa(pa)
        , pb(pb)
        , pc(pc)
        , pd(pd) {};

    constexpr Point multiply(const Point& point) const
    {
        return {
            (pa * point.x + pb * point.y) >> 8,
            (pc * point.x + pd * point.y) >> 8
        };
    }

    int pa;
    int pb;
    int pc;
    int pd;
};
