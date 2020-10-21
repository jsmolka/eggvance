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

class Layer
{
public:
    bool operator<=(const Layer& other) const { return priority <= other.priority; }
    bool operator< (const Layer& other) const { return priority <  other.priority; }

    uint priority = 4;
};

class BgLayer : public Layer
{
public:
    bool operator<=(const BgLayer& other) const { return priority == other.priority ? flag <= other.flag : priority <= other.priority; }
    bool operator< (const BgLayer& other) const { return priority == other.priority ? flag <  other.flag : priority <  other.priority; }

    u16 color(uint x) const
    {
        return data[x];
    }

    bool opaque(uint x) const
    {
        return color(x) != kTransparent;
    }

    u16* data = nullptr;
    uint flag = 0;
};

class ObjectLayer : public Layer
{
public:
    bool opaque() const
    {
        return color != kTransparent;
    }

    u16  color  = kTransparent;
    bool window = false;
    bool alpha  = false;
};
