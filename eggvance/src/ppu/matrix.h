#pragma once

#include "common/integer.h"
#include "point.h"

struct Matrix
{
    constexpr Matrix(s16 pa, s16 pb, s16 pc, s16 pd)
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

    s16 pa;
    s16 pb;
    s16 pc;
    s16 pd;
};

static constexpr Matrix identity_matrix = { 0x100, 0x000, 0x000, 0x100 };
