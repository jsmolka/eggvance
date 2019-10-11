#include "backgroundcontrol.h"

#include "common/macros.h"
#include "common/utility.h"

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

u8 BackgroundControl::readByte(int index)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
        byte |= priority     << 0;
        byte |= tile_block   << 2;
        byte |= mosaic       << 6;
        byte |= palette_type << 7;
        break;

    case 1:
        byte |= map_block   << 0;
        byte |= wraparound  << 5;
        byte |= screen_size << 6;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

void BackgroundControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index == 0 || index == 1, "Invalid index");

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

    default:
        EGG_UNREACHABLE;
        break;
    }
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
