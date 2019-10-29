#pragma once

#include "common/integer.h"
#include "common/constants.h"

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
    Layer(int prio) : prio(prio) { }

    inline bool operator<(const Layer& other) const
    {
        return prio < other.prio;
    }

    inline bool operator<=(const Layer& other) const
    {
        return prio <= other.prio;
    }

    int prio;
};

struct BackgroundLayer : public Layer
{
    BackgroundLayer(int prio, u16* data, int flag)
        : Layer(prio)
        , data(data)
        , flag(flag) { }

    inline bool operator<(const BackgroundLayer& other) const
    {
        if (prio == other.prio)
            return flag < other.flag;
        else    
            return prio < other.prio;
    }

    inline u16 color(int x) const
    {
        return data[x];
    }

    inline bool opaque(int x) const
    {
        return data[x] != TRANSPARENT;
    }

    u16* data;
    int  flag;
};

struct ObjectLayer : public Layer
{
    ObjectLayer() : Layer(4) { }

    u16  color  = TRANSPARENT;
    bool opaque = false;
    bool window = false;
    bool alpha  = false;
};
