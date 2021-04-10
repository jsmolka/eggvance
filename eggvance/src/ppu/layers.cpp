#include "layers.h"

bool Layer::operator<=(const Layer& other) const
{
    return priority <= other.priority;
}

bool BackgroundLayer::operator<(const BackgroundLayer& other) const
{
    return ((priority << 8) | flag) < ((other.priority << 8) | other.flag);
}

u16 BackgroundLayer::color(uint x) const
{
    return data[x];
}

bool BackgroundLayer::opaque(uint x) const
{
    return color(x) != kTransparent;
}

bool ObjectLayer::opaque() const
{
    return color != kTransparent;
}
