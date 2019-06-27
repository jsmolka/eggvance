#pragma once

enum
{
    COLOR_TRANSPARENT = 0x8000
};

struct ObjPixel
{
    ObjPixel(
        int color = COLOR_TRANSPARENT, int prio = 4, int semi = 0)
        : color(color)
        , prio(prio)
        , semi(semi) 
    {

    }

    int color;
    int prio;
    int semi;
};
