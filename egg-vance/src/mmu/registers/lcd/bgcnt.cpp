#include "bgcnt.h"

#include "common/memory_map.h"

Bgcnt::Bgcnt(u16& data)
    : data(data)
    , priority(data)
    , tile_base(data)
    , mosaic(data)
    , palette(data)
    , map_base(data)
    , wraparound(data)
    , screen_size(data)
{

}

u32 Bgcnt::tileAddr() const
{
    return MAP_VRAM + 0x4000 * tile_base;
}

u32 Bgcnt::mapAddr() const
{
    return MAP_VRAM + 0x800 * map_base;
}
