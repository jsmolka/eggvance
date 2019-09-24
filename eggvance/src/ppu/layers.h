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

struct Layer
{
    u16 color(int x) const
    {
        return data[x];
    }

    bool opaque(int x) const
    {
        return data[x] != TRANSPARENT;
    }

    bool operator<(const Layer& other) const
    {
        if (prio == other.prio)
            return id < other.id;
        else
            return prio < other.prio;
    }

    int  id;
    u16* data;
    int  prio;
    int  flag;
};

struct ObjectData
{
    ObjectData()
        : color(TRANSPARENT)
        , opaque(false)
        , prio(4)
        , alpha(false)
        , window(false)
    { 

    }

    bool precedes(const Layer& layer) const
    {
        return prio <= layer.prio;
    }

    int  color;
    bool opaque;
    int  prio;
    bool alpha;
    bool window;
};
