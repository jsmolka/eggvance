#pragma once

#include "constants.h"
#include "base/int.h"

class Layer
{
public:
    enum class Flag
    {
        Bg0 = 1 << 0,
        Bg1 = 1 << 1,
        Bg2 = 1 << 2,
        Bg3 = 1 << 3,
        Obj = 1 << 4,
        Bdp = 1 << 5
    };

    bool operator<=(const Layer& other) const;

    uint priority = 4;
};

class BgLayer : public Layer
{
public:
    bool operator<(const BgLayer& other) const;

    u16 color(uint x) const;
    bool opaque(uint x) const;

    u16* data = nullptr;
    uint flag = 0;
};

class ObjectLayer : public Layer
{
public:
    bool opaque() const;

    u16  color  = kTransparent;
    bool alpha  = false;
    bool window = false;
};
