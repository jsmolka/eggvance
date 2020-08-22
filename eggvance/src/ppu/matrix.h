#pragma once

#include "point.h"
#include "base/eggcpt.h"

class Matrix
{
public:
    constexpr Matrix(s16 pa, s16 pb, s16 pc, s16 pd)
        : pa(pa)
        , pb(pb)
        , pc(pc)
        , pd(pd)
    {
    
    }

    constexpr Point multiply(const Point& point) const
    {
        return Point(
            pa * point.x + pb * point.y,
            pc * point.x + pd * point.y
        );
    }

private:
    int pa;
    int pb;
    int pc;
    int pd;
};

static constexpr Matrix identity_matrix(0x100, 0, 0, 0x100);
