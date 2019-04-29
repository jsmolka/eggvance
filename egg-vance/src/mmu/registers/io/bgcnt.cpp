#include "bgcnt.h"

#include "common/memory_map.h"

Bgcnt::Bgcnt(u16& data)
    : priority(data)
    , tile_base_block(data)
    , mosaic(data)
    , palette(data)
    , map_base_block(data)
    , disp_overflow(data)
    , screen_size(data)
{

}

u32 Bgcnt::tileAddr() const
{
    return MAP_VRAM + 0x4000 * tile_base_block;
}

u32 Bgcnt::mapAddr() const
{
    return MAP_VRAM + 0x800 * map_base_block;
}
