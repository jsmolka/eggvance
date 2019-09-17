#pragma once

#include <vector>

#include "common/integer.h"
#include "constants.h"

// Todo: move into layer?
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
    Layer(int id, u16* data, int prio, int flag)
        : id(id), data(data), prio(prio), flag(flag) {};

    inline u16 color(int x) const
    {
        return data[x];
    }

    inline bool opaque(int x) const
    {
        return data[x] != TRANSPARENT;
    }

    int  id;
    u16* data;
    int  prio;
    int  flag;
};
