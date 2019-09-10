#pragma once

#include "constants.h"
#include "layers.h"

struct ObjectData
{
    ObjectData()
        : color(COLOR_T)
        , opaque(false)
        , prio(4)
        , alpha(false)
        , window(false)
    { 
    
    }

    inline bool precedes(const Layer& layer) const
    {
        return prio <= layer.prio;
    }

    int  color;
    bool opaque;
    int  prio;
    bool alpha;
    bool window;
};
