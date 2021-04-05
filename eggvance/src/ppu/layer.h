#pragma once

#include "constants.h"
#include "base/int.h"

class Layer
{
public:
    enum class Flag
    {
        Non = 0,
        Bg0 = 1 << 0,
        Bg1 = 1 << 1,
        Bg2 = 1 << 2,
        Bg3 = 1 << 3,
        Obj = 1 << 4,
        Bdp = 1 << 5
    };

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
    bool alpha  = false;
    bool window = false;
};
