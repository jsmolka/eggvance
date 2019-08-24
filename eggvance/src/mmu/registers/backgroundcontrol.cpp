#include "backgroundcontrol.h"

static constexpr int sizes[4][2] =
{
    { 256, 256 },
    { 512, 256 },
    { 256, 512 },
    { 512, 512 }
};

void BackgroundControl::reset()
{
    priority     = 0;
    tile_block   = 0;
    mosaic       = 0;
    palette_type = 0;
    map_block    = 0;
    wraparound   = 0;
    screen_size  = 0;
}

int BackgroundControl::size() const
{
    return 128 << screen_size;
}

int BackgroundControl::width() const
{
    return sizes[screen_size][0];
}

int BackgroundControl::height() const
{
    return sizes[screen_size][1];;
}
