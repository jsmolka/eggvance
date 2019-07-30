#include "BackgroundControl.h"

u32 BackgroundControl::tileBase() const
{
    return 0x4000 * tile_block;
}

u32 BackgroundControl::mapBase() const
{
    return 0x800 * map_block;
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
