#pragma once

#include "base/int.h"
#include "gpu/point.h"

class Matrix
{
public:
    constexpr Matrix(s16 pa, s16 pb, s16 pc, s16 pd)
        : pa(pa), pb(pb), pc(pc), pd(pd) {}

    constexpr Point operator*(const Point& point) const
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

inline constexpr Matrix kIdentityMatrix(0x100, 0, 0, 0x100);
