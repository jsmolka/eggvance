#pragma once

#include "constants.h"

struct ObjectData
{
    ObjectData()
        : color(COLOR_T)
        , priority(4)
        , mode(GFX_NORMAL)
        , window(0) 
    {
    
    }

    int color;
    int priority;
    int mode;
    int window;
};
