#include "dmacontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void DMAControl::reset()
{
    *this = {};
}

u8 DMAControl::read(int index)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
        return data[0];
    else
        return (data[1] & ~0x80) | (enabled << 7);
}

void DMAControl::write(int index, u8 byte)
{
    EGG_ASSERT(index <= 1, "Invalid index");

    if (index == 0)
    {
        dad_delta = bits<5, 2>(byte);
        sad_delta = bits<7, 1>(byte) << 0 | (sad_delta & ~0x1);
    }
    else
    {
        sad_delta = bits<0, 1>(byte) << 1 | (sad_delta & ~0x2);
        repeat    = bits<1, 1>(byte);
        word      = bits<2, 1>(byte);
        timing    = bits<4, 2>(byte);
        irq       = bits<6, 1>(byte);
        reload    = bits<7, 1>(byte) != enabled;
        enabled   = bits<7, 1>(byte);
    }
    data[index] = byte;
}
