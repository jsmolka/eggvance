#include "dmacnt.h"

void DMACnt::reset()
{
    sad_delta = 0;
    dad_delta = 0;
    repeat   = 0;
    word     = 0;
    drq      = 0;
    timing   = 0;
    irq      = 0;
    enabled   = 0;
    count    = 0;
}
