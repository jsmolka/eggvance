#pragma once

#include <vector>

#include "common/integer.h"
#include "constants.h"

enum LayerFlag
{
    LF_BG0 = 1 << 0,
    LF_BG1 = 1 << 1,
    LF_BG2 = 1 << 2,
    LF_BG3 = 1 << 3,
    LF_OBJ = 1 << 4,
    LF_BDP = 1 << 5
};

struct Layer
{
    inline bool set(u16* scanline, int x) const
    {
        if (data[x] != COLOR_T)
        {
            scanline[x] = data[x];
            return true;
        }
        return false;
    }
    u16* data;
    u16  prio;
    int  flag;
};

struct Layers
{
    inline void sort()
    {
        std::sort(data.begin(), data.end(), [](const Layer& lhs, const Layer& rhs) {
            if (lhs.prio == rhs.prio)
                return lhs.flag < rhs.flag;
            return lhs.prio < rhs.prio;
        });
    }
    std::vector<Layer> data;
};
