#include "displaycontrol.h"

void DisplayControl::reset()
{
    mode        = 0;
    frame       = 0;
    oam_hblank  = 0;
    mapping_1d  = 0;
    force_blank = 0;
    obj         = 0;
    win0        = 0;
    win1        = 0;
    winobj      = 0;
}
