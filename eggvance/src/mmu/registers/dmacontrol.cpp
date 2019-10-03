#include "dmacontrol.h"

void DMAControl::reset()
{
    sad_control = 0;
    dad_control = 0;
    repeat     = 0;
    word       = 0;
    drq        = 0;
    timing     = 0;
    irq        = 0;
    enable     = 0;
    count      = 0;
}
