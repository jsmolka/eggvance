#pragma once

#include "base/integer.h"
#include "base/constants.h"

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
    constexpr Layer(int prio)
        : prio(prio) 
    {
    
    }

    constexpr bool operator<(const Layer& other) const
    {
        return prio < other.prio;
    }

    constexpr bool operator<=(const Layer& other) const
    {
        return prio <= other.prio;
    }

    int prio;
};

struct BGLayer : public Layer
{
    constexpr BGLayer(int prio, u16* data, int flag)
        : Layer(prio)
        , data(data)
        , flag(flag)
    {
    
    }

    constexpr bool operator<(const BGLayer& other) const
    {
        return prio == other.prio
            ? flag < other.flag
            : prio < other.prio;
    }

    constexpr bool opaque(int x) const
    {
        return data[x] != kTransparent;
    }

    constexpr u16 color(int x) const
    {
        return data[x];
    }

    u16* data;
    int  flag;
};

struct ObjectLayer : public Layer
{
    constexpr ObjectLayer()
        : Layer(4)
        , color(kTransparent)
        , window(false)
        , alpha(false)
    {
    
    }

    constexpr bool opaque() const
    {
        return color != kTransparent;
    }

    u16  color;
    bool window;
    bool alpha;
};
