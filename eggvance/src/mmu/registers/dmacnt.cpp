#include "dmacnt.h"

void DMACnt::reset()
{
    sad_ctrl = 0;
    dad_ctrl = 0;
    repeat   = 0;
    word     = 0;
    drq      = 0;
    timing   = 0;
    irq      = 0;
    enable   = 0;
    count    = 0;
}
