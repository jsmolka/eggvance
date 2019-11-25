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
        pal_format = bits<7, 1>(byte);
    }
    else
    {
        map_block   = bits<0, 5>(byte);
        wraparound  = bits<5, 1>(byte);
        screen_size = bits<6, 2>(byte);
    }
    data[index] = byte;

    update();
}

void BGControl::update()
{
    static constexpr Dimensions dims[4] =
    {
        { 256, 256 },
        { 512, 256 },
        { 256, 512 },
        { 512, 512 }
    };

    dims_reg   = dims[screen_size];
    dims_aff.w = 128 << screen_size;
    dims_aff.h = 128 << screen_size;

    base_map  = 0x0800 * map_block;
    base_tile = 0x4000 * tile_block;;
}
