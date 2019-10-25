#include "bgcontrol.h"

#include "common/macros.h"
#include "common/utility.h"

int BGControl::sizes[4][2] =
{
    { 256, 256 },
    { 512, 256 },
    { 256, 512 },
    { 512, 512 }
};

void BGControl::reset()
{
    *this = {};
}

u8 BGControl::readByte(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");
    return bytes[index];
}

void BGControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        priority   = bits<0, 2>(byte);
        tile_block = bits<2, 2>(byte);
        mosaic     = bits<6, 1>(byte);
        pformat    = bits<7, 1>(byte);
    }
    else
    {
        map_block   = bits<0, 5>(byte);
        wraparound  = bits<5, 1>(byte);
        screen_size = bits<6, 2>(byte);
    }
    bytes[index] = byte;
}

int BGControl::size() const
{
    return 128 << screen_size;
}

int BGControl::width() const
{
    return sizes[screen_size][0];
}

int BGControl::height() const
{
    return sizes[screen_size][1];
}
