#pragma once

#include "constants.h"
#include "ppu/oamentry.h"

// For sorting lower than backdrop
struct ObjectData
{
    ObjectData()
        : color(COLOR_T)
        , priority(5)
        , mode(GFX_NORMAL)
        , window(0) 
    {
    
    }

    inline bool opaque() const
    {
        return color != COLOR_T;
    }

    inline bool qualified(int prio) const
    {
        return priority <= prio && opaque();
    }

    int color;
    int priority;
    int mode;
    int window;
};
