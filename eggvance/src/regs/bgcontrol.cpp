#include "bgcontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void BGControl::reset()
{
    *this = {};
}

u8 BGControl::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    return data[index];
}

void BGControl::write(int index, u8 byte)
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

        updateDims();
    }
    data[index] = byte;
}

u32 BGControl::mapBase() const
{
    return 0x800 * map_block;
}

u32 BGControl::tileBase() const
{
    return 0x4000 * tile_block;
}

void BGControl::updateDims()
{
    static constexpr int sizes[4][2] =
    {
        { 256, 256 },
        { 512, 256 },
        { 256, 512 },
        { 512, 512 }
    };

    dims_reg.w = sizes[screen_size][0];
    dims_reg.h = sizes[screen_size][1];
    dims_aff.w = 128 << screen_size;
    dims_aff.h = 128 << screen_size;
}
