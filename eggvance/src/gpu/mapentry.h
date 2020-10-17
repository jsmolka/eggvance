#pragma once

#include "base/bit.h"
#include "base/int.h"

struct MapEntry
{
    MapEntry(u16 data)
    {
        tile   = bit::seq< 0, 10>(data);
        flip_x = bit::seq<10,  1>(data);
        flip_y = bit::seq<11,  1>(data);
        bank   = bit::seq<12,  4>(data);
    }

    uint tile;
    uint flip_x;
    uint flip_y;
    uint bank;
};
