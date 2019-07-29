#include "BackgroundControl.h"

#include "mmu/memory.h"

u32 BackgroundControl::tileBase() const
{
    return MAP_VRAM + 0x4000 * tile_base;
}

u32 BackgroundControl::mapBase() const
{
    return MAP_VRAM + 0x800 * map_base;
}

int BackgroundControl::width() const
{
    return sizes[screen_size][0];
}

int BackgroundControl::height() const
{
    return sizes[screen_size][1];;
}

int BackgroundControl::affineSize() const
{
    return affine_sizes[screen_size];
}

const int BackgroundControl::sizes[4][2] =
{
    { 256, 256 },
    { 512, 256 },
    { 256, 512 },
    { 512, 512 }
};

const int BackgroundControl::affine_sizes[4] =
{
     128,
     256,
     512,
    1024
};
