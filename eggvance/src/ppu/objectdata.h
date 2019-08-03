#pragma once

#include "constants.h"
#include "enums.h"

struct ObjectData
{
    ObjectData()
        : color(TRANSPARENT)
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
