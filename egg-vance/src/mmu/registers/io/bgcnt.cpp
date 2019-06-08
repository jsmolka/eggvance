#include "bgcnt.h"

#include "mmu/map.h"

Bgcnt::Bgcnt(u16& data)
    : Register<u16>(data)
    , priority(data)
    , tile_base(data)
    , mosaic(data)
    , palette_type(data)
    , map_base(data)
    , wraparound(data)
    , screen_size(data)
{

}

u32 Bgcnt::tileBase() const
{
    return MAP_VRAM + tile_block_size * tile_base;
}

u32 Bgcnt::mapBase() const
{
    return MAP_VRAM + map_block_size * map_base;
}

int Bgcnt::width() const
{
    return sizes[screen_size][0];
}

int Bgcnt::height() const
{
    return sizes[screen_size][1];;
}

int Bgcnt::affineSize() const
{
    return affine_sizes[screen_size];
}

const int Bgcnt::sizes[4][2] =
{
    { 256, 256 },
    { 512, 256 },
    { 256, 512 },
    { 512, 512 }
};

const int Bgcnt::affine_sizes[4] =
{
     128,
     256,
     512,
    1024
};
