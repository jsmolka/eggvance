#include "dispstat.h"

Dispstat::Dispstat(u16& data)
    : Register<u16>(data)
    , vblank(data)
    , hblank(data)
    , vcount_match(data)
    , vblank_irq(data)
    , hblank_irq(data)
    , vcount_irq(data)
    , vcount_compare(data)
{

}
