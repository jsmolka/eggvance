#include "backgroundcontrol.h"

#include "common/utility.h"

static constexpr int sizes[4][2] =
{
    { 256, 256 },
    { 512, 256 },
    { 256, 512 },
    { 512, 512 }
};

void BackgroundControl::write(int index, u8 byte)
{
    switch (index)
    {
    case 0:
        priority     = bits<0, 2>(byte);
        tile_block   = bits<2, 2>(byte);
        mosaic       = bits<6, 1>(byte);
        palette_type = bits<7, 1>(byte);
        break;

    case 1:
        map_block   = bits<0, 5>(byte);
        wraparound  = bits<5, 1>(byte);
        screen_size = bits<6, 2>(byte);
        break;
    }
}

int BackgroundControl::size() const
{
    return 1 << (7 + screen_size);
}

int BackgroundControl::width() const
{
    return sizes[screen_size][0];
}

int BackgroundControl::height() const
{
    return sizes[screen_size][1];;
}
