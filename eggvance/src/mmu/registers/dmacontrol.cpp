#include "dmacontrol.h"

void DMAControl::reset()
{
    count       = 0;
    src_adjust  = 0;
    dst_adjust  = 0;
    repeat      = 0;
    word        = 0;
    drq         = 0;
    timing      = 0;
    irq         = 0;
    enable      = 0;
}
