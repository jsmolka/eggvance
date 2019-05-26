#include "bgcnt.h"

#include "mmu/map.h"

Bgcnt::Bgcnt(u16& data)
    : Register<u16>(data)
    , priority(data)
    , tile_block(data)
    , mosaic(data)
    , palette_type(data)
    , map_block(data)
    , wraparound(data)
    , screen_size(data)
{

}

u32 Bgcnt::tileAddr() const
{
    return MAP_VRAM + tile_block_size * tile_block;
}

u32 Bgcnt::mapAddr() const
{
    return MAP_VRAM + map_block_size * map_block;
}

int Bgcnt::tilesX() const
{
    return tiles[screen_size][0];
}

int Bgcnt::tilesY() const
{
    return tiles[screen_size][1];;
}

int Bgcnt::affineTilesX() const
{
    return affine_tiles[screen_size][0];
}

int Bgcnt::affineTilesY() const
{
    return affine_tiles[screen_size][1];
}

const int Bgcnt::tiles[4][2] =
{
    { 32, 32 },
    { 64, 32 },
    { 32, 64 },
    { 64, 64 }
};

const int Bgcnt::affine_tiles[4][2] =
{
    {  16,  16 },
    {  32,  32 },
    {  64,  64 },
    { 128, 128 }
};
