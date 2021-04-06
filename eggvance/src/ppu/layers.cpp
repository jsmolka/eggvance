#include "layers.h"

bool Layer::operator<=(const Layer& other) const
{
    return priority <= other.priority;
}

bool BgLayer::operator<(const BgLayer& other) const
{
    return ((priority << 8) | flag) < ((other.priority << 8) | other.flag);
}

u16 BgLayer::color(uint x) const
{
    return data[x];
}

bool BgLayer::opaque(uint x) const
{
    return color(x) != kTransparent;
}

bool ObjectLayer::opaque() const
{
    return color != kTransparent;
}
