#include "dmacontrol.h"

#include "common/macros.h"
#include "common/utility.h"

void DMAControl::reset()
{
    int mask = count_mask;
    *this = {};
    count_mask = mask;
}

u8 DMAControl::readByte(int index)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    u8 byte = 0;
    switch (index)
    {
    case 0:
    case 1:
        break;

    case 2:
        byte = bytes[2];
        break;

    case 3:
        byte = bytes[3] & ~0x80;
        byte |= enabled << 7;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    return byte;
}

void DMAControl::writeByte(int index, u8 byte)
{
    EGG_ASSERT(index <= 3, "Invalid index");

    switch (index)
    {
    case 0:
    case 1:
        byteArray(count)[index] = byte;
        count &= count_mask;
        if (count == 0)
            count = count_mask + 1;
        break;

    case 2:
        dad_delta = bits<5, 2>(byte);
        sad_delta = bits<7, 1>(byte) << 0 | (sad_delta & ~0x1);
        break;

    case 3:
        sad_delta = bits<0, 1>(byte) << 1 | (sad_delta & ~0x2);
        repeat    = bits<1, 1>(byte);
        word      = bits<2, 1>(byte);
        timing    = bits<4, 2>(byte);
        irq       = bits<6, 1>(byte);
        enabled   = bits<7, 1>(byte);
        update    = enabled;
        break;

    default:
        EGG_UNREACHABLE;
        break;
    }
    bytes[index] = byte;
}
