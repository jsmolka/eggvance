#pragma once

#include "point.h"
#include "base/int.h"

class MapEntry
{
public:
    MapEntry(u16 data);

    uint  tile;
    uint  bank;
    Point flip;
};
