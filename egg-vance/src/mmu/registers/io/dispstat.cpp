#include "dispstat.h"

Dispstat::Dispstat(u16& data)
    : vblank_flag(data)
    , hblank_flag(data)
    , vcount_flag(data)
    , vblank_enable(data)
    , hblank_enable(data)
    , vcount_enable(data)
    , vcount_trigger(data)
{

}
