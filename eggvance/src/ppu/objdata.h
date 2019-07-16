#pragma once

#include "constants.h"
#include "enums.h"

struct ObjData
{
    ObjData(int color = TRANSPARENT, int priority = 4, GraphicsMode mode = GFX_NORMAL, int window = 0)
        : color(color), priority(priority), mode(mode), window(window) { }

    int color;
    int priority;
    GraphicsMode mode;
    int window;
};
