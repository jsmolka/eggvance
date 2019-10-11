#pragma once

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

struct BackgroundLayer
{
    BackgroundLayer(int id, u16* data, int priority)
        : id(id)
        , data(data)
        , flag(1 << id)
        , priority(priority)
    {

    }

    inline bool operator<(const BackgroundLayer& other) const
    {
        return priority < other.priority;
    }

    inline bool operator<=(const BackgroundLayer& other) const
    {
        return priority <= other.priority;
    }

    inline u16 color(int x) const
    {
        return data[x];
    }

    inline bool opaque(int x) const
    {
        return color(x) != TRANSPARENT;
    }

    int  id;
    u16* data;
    int  flag;
    int  priority;
};
