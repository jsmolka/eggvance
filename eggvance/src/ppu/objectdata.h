#pragma once

#include "constants.h"
#include "ppu/oamentry.h"

// For sorting lower than backdrop
struct ObjectData
{
    ObjectData()
        : color(COLOR_T)
        , prio(4)
        , mode(GFX_NORMAL)
        , window(0) 
    {
    
    }

    inline bool opaque() const
    {
        return color != COLOR_T;
    }

    inline bool visible(int prio = 3) const
    {
        return this->prio <= prio && opaque();
    }

    int color;
    int prio;
    int mode;
    int window;
};
