#pragma once

#include "ppu/enums.h"

struct Window
{
    int bg0;  // BG0 enabled
    int bg1;  // BG1 enabled
    int bg2;  // BG2 enabled
    int bg3;  // BG3 enabled
    int obj;  // OBJ enabled
    int sfx;  // SFX enabled

    int mask;
    inline void makeMask()
    {
        mask = 0;
        if (bg0) mask |= LF_BG0;
        if (bg1) mask |= LF_BG1;
        if (bg2) mask |= LF_BG2;
        if (bg3) mask |= LF_BG3;
        if (obj) mask |= LF_OBJ;
    }
};
