#include "displaycontrol.h"

void DisplayControl::reset()
{
    mode        = 0;
    frame       = 0;
    oam_hblank  = 0;
    mapping_1d  = 0;
    force_blank = 0;
    bg0         = 0;
    bg1         = 0;
    bg2         = 0;
    bg3         = 0;
    obj         = 0;
    win0        = 0;
    win1        = 0;
    winobj      = 0;
}

bool DisplayControl::enabled() const
{
    switch (mode)
    {
    case 0: return bg0 || bg1 || bg2 || bg3 || obj;
    case 1: return bg0 || bg1 || bg2 || obj;
    case 2: return bg2 || bg3 || obj;
    case 3: return bg2 || obj;
    case 4: return bg2 || obj;
    case 5: return bg2 || obj; 
    }
    return false;
}
