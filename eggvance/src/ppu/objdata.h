#pragma once

#include "constants.h"
#include "enums.h"

struct ObjData
{
    ObjData(int color = TRANSPARENT, int priority = 4, GraphicsMode mode = GFX_NORMAL)
        : color(color), priority(priority), mode(mode) { }

    int color;
    int priority;
    GraphicsMode mode;
};
