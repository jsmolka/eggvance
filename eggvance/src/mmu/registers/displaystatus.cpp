#include "displaystatus.h"

void DisplayStatus::reset()
{
    vblank         = 0;
    hblank         = 0;
    vmatch         = 0;
    vblank_irq     = 0;
    hblank_irq     = 0;
    vmatch_irq     = 0;
    vcount_compare = 0;
}
