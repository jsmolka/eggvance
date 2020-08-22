#pragma once

#include "base/eggcpt.h"

struct MapEntry
{
    MapEntry(u16 data);

    int tile;
    int flip_x;
    int flip_y;
    int bank;
};
