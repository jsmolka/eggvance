#pragma once

#include "constants.h"
#include "base/int.h"

enum LayerFlag
{
    kLayerBg0 = 1 << 0,
    kLayerBg1 = 1 << 1,
    kLayerBg2 = 1 << 2,
    kLayerBg3 = 1 << 3,
    kLayerObj = 1 << 4,
    kLayerBdp = 1 << 5
};

struct Layer
{
    Layer(uint prio)
        : prio(prio) {}

    bool operator<=(const Layer& other) const { return prio <= other.prio; }
    bool operator< (const Layer& other) const { return prio <  other.prio; }

    uint prio;
};

struct BgLayer : Layer
{
    BgLayer(uint prio, u16* data, uint flag)
        : Layer(prio), data(data), flag(flag) {}

    bool operator<=(const BgLayer& other) const { return prio == other.prio ? flag <= other.flag : prio <= other.prio; }
    bool operator< (const BgLayer& other) const { return prio == other.prio ? flag <  other.flag : prio <  other.prio; }

    u16 color(uint x) const
    {
        return data[x];
    }

    bool opaque(uint x) const
    {
        return color(x) != kTransparent;
    }

    u16* data;
    uint flag;
};

struct ObjectLayer : Layer
{
    ObjectLayer()
        : Layer(4) {}

    bool opaque() const
    {
        return color != kTransparent;
    }

    u16  color  = kTransparent;
    bool window = false;
    bool alpha  = false;
};
